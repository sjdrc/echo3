package au.com.emblementertainment.echo;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.app.Activity;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class EchoView extends GLSurfaceView implements SensorEventListener
{
	private static String TAG = "EchoView";
	private static final boolean DEBUG = false;
	
	private AssetManager mAssetManager;
	private String mInternalApplicationDirectory;
	private float mScreenXDIP;
	private float mScreenYDIP;
	private Activity mActivity;		//Activity that will be "finished" when Echo.Update() returns false.
	
	private Display mDisplay;

	private SensorManager mSensorManager;
	private Sensor mAccelerometer;
	private Sensor mGravity;
	private Sensor mGyroscope;
	private float[] gravity = new float[3];

	// Create a constant to convert nanoseconds to seconds.
	private static final float NS2S = 1.0f / 1000000000.0f;
	private final float[] deltaRotationVector = new float[4];
	private final float[] rotationCurrent = new float[4];
	private long timestamp;
	
	public EchoView(Context context, Display display, AssetManager assetManager, String internalApplicationDirectory, float screenXDIP, float screenYDIP, Activity activity)
	{
		super(context);
		mDisplay = display;
		mAssetManager = assetManager;
		mInternalApplicationDirectory = internalApplicationDirectory;
		mScreenXDIP = screenXDIP;
		mScreenYDIP = screenYDIP;
		mActivity = activity;
		
		deltaRotationVector[0] = 0;
		deltaRotationVector[1] = 0;
		deltaRotationVector[2] = 0;
		deltaRotationVector[3] = 0;
		timestamp = 0;
		gravity[0] = 0;
		gravity[1] = 0;
		gravity[2] = 0;
		rotationCurrent[0] = 1.f;
		rotationCurrent[1] = 0.f;
		rotationCurrent[2] = 0.f;
		rotationCurrent[3] = 0.f;
		
		int screenRotation = mDisplay.getRotation();
		switch(screenRotation)
		{
			case Surface.ROTATION_0:
			break;
			case Surface.ROTATION_90:
				rollQuaternionByAngle(rotationCurrent,(float) (Math.PI/2.f),rotationCurrent);
			break;
			case Surface.ROTATION_180:
				rollQuaternionByAngle(rotationCurrent,(float) Math.PI,rotationCurrent);
			break;
			case Surface.ROTATION_270:
				rollQuaternionByAngle(rotationCurrent,(float) (2.f*Math.PI/3.f),rotationCurrent);
			break;
		}
		
		mSensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
		mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		mGravity = mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY);
		mGyroscope = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

		//setEGLContextClientVersion(2);
		init(true, 0, 0);
		setRenderMode(RENDERMODE_CONTINUOUSLY);
	}

	public EchoView(Context context, boolean translucent, int depth,
			int stencil)
	{
		super(context);
		init(translucent, depth, stencil);
	}

	private void init(boolean translucent, int depth, int stencil)
	{

		/*
		 * By default, GLSurfaceView() creates a RGB_565 opaque surface. If we
		 * want a translucent one, we should change the surface's format here,
		 * using PixelFormat.TRANSLUCENT for GL Surfaces is interpreted as any
		 * 32-bit surface with alpha by SurfaceFlinger.
		 */
		if (translucent)
		{
			this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		}

		/*
		 * Setup the context factory for 2.0 rendering. See ContextFactory class
		 * definition below
		 */
		setEGLContextFactory(new ContextFactory());

		/*
		 * We need to choose an EGLConfig that matches the format of our surface
		 * exactly. This is going to be done in our custom config chooser. See
		 * ConfigChooser class definition below.
		 */
		setEGLConfigChooser(translucent ? new ConfigChooser(8, 8, 8, 8, depth,
				stencil) : new ConfigChooser(5, 6, 5, 0, depth, stencil));
		setBackgroundColor(0x00000000);
		
		getHolder().setFormat(PixelFormat.RGBA_8888);
		setZOrderOnTop(true);

		/* Set the renderer responsible for frame rendering */
		setRenderer(new Renderer(mAssetManager,mInternalApplicationDirectory,mScreenXDIP,mScreenYDIP,mActivity));
	}
	
	void concatQuaternion(float[] quaternion, float[] rotateBy)
	{
		float[] newRotation = new float[4];
		
		newRotation[0] = quaternion[0] * rotateBy[0] - quaternion[1] * rotateBy[1] - quaternion[2] * rotateBy[2] - quaternion[3] * rotateBy[3];		//W
		newRotation[1] = quaternion[0] * rotateBy[1] + quaternion[1] * rotateBy[0] + quaternion[2] * rotateBy[3] - quaternion[3] * rotateBy[2];		//X
		newRotation[2] = quaternion[0] * rotateBy[2] + quaternion[2] * rotateBy[0] + quaternion[3] * rotateBy[1] - quaternion[1] * rotateBy[3];		//Y
		newRotation[3] = quaternion[0] * rotateBy[3] + quaternion[3] * rotateBy[0] + quaternion[1] * rotateBy[2] - quaternion[2] * rotateBy[1];		//Z
		
		quaternion[0] = newRotation[0];
		quaternion[1] = newRotation[1];
		quaternion[2] = newRotation[2];
		quaternion[3] = newRotation[3];
	}
	
	void rollQuaternionByAngle(float[] quaternion, float rollInRadians, float[] destination)
	{
		float yaw = 0;
		float pitch = 0;
		float roll = rollInRadians;
		
		float halfYaw = yaw * 0.5f; 
		float halfPitch = pitch * 0.5f;
		float halfRoll = roll * 0.5f;
		float cosYaw = (float) Math.cos(halfYaw);
		float sinYaw = (float) Math.sin(halfYaw);
		float cosPitch = (float) Math.cos(halfPitch);
		float sinPitch = (float) Math.sin(halfPitch);
		float cosRoll = (float) Math.cos(halfRoll);
		float sinRoll = (float) Math.sin(halfRoll);
		float[] rotation = new float[4];
		rotation[0] = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
		rotation[1] = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
		rotation[2] = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
		rotation[3] = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
		
		destination[0] = quaternion[0];
		destination[1] = quaternion[1];
		destination[2] = quaternion[2];
		destination[3] = quaternion[3];
		concatQuaternion(destination,rotation);
	}
	
	public boolean onTouchEvent(MotionEvent e)
	{
		// MotionEvent reports input details from the touch screen
		// and other input controls. In this case, you are only
		// interested in events where the touch position changed.

		switch (e.getAction())
		{
		case MotionEvent.ACTION_UP:
			Echo.SetMouse(e.getX(),e.getY(),0);
			break;
		case MotionEvent.ACTION_DOWN:
			Echo.SetMouse(e.getX(),e.getY(),1);
			break;
		case MotionEvent.ACTION_MOVE:
			Echo.SetMouse(e.getX(),e.getY(),1);
			break;
		}
		return true;
	}
	
	//Sensors
	public void onSensorChanged(SensorEvent event)
	{
		switch(event.sensor.getType())
		{
			case Sensor.TYPE_ACCELEROMETER:
			{	// In this example, alpha is calculated as t / (t + dT),
				// where t is the low-pass filter's time-constant and
				// dT is the event delivery rate.
		
				final float alpha = 0.8f;
		
				// Isolate the force of gravity with the low-pass filter.
				gravity[0] = alpha * gravity[0] + (1 - alpha) * event.values[0];
				gravity[1] = alpha * gravity[1] + (1 - alpha) * event.values[1];
				gravity[2] = alpha * gravity[2] + (1 - alpha) * event.values[2];
		
				// Remove the gravity contribution with the high-pass filter.
				float x = event.values[0] - gravity[0];
				float y = event.values[1] - gravity[1];
				float z = event.values[2] - gravity[2];
				Echo.SetLinearAcceleration(x,y,z);
			}break;
			case Sensor.TYPE_GRAVITY:
			{
				Echo.SetGravity(event.values[0],event.values[1],event.values[2]);
			}break;
			case Sensor.TYPE_GYROSCOPE:
			{
				// This timestep's delta rotation to be multiplied by the current rotation
				// after computing it from the gyro sample data.
				if (timestamp != 0)
				{
					long timeStampDelta = (event.timestamp - timestamp);
					final float dT = timeStampDelta * NS2S;
					// Axis of the rotation sample, not normalized yet.
					float axisX = event.values[0];
					float axisY = event.values[1];
					float axisZ = event.values[2];

					// Calculate the angular speed of the sample
					float omegaMagnitude = (float) Math.sqrt(axisX*axisX + axisY*axisY + axisZ*axisZ);
				
					// Normalize the rotation vector if it's big enough to get the axis
					// (that is, EPSILON should represent your maximum allowable margin of error)
					final float EPSILON = 0.005f;
					if (omegaMagnitude > EPSILON)
					{
						axisX /= omegaMagnitude;
						axisY /= omegaMagnitude;
						axisZ /= omegaMagnitude;
					}

					// Integrate around this axis with the angular speed by the timestep
					// in order to get a delta rotation from this sample over the timestep
					// We will convert this axis-angle representation of the delta rotation
					// into a quaternion before turning it into the rotation matrix.
					float thetaOverTwo = omegaMagnitude * dT / 2.0f;
					float sinThetaOverTwo = (float) Math.sin(thetaOverTwo);
					float cosThetaOverTwo = (float) Math.cos(thetaOverTwo);
					deltaRotationVector[0] = sinThetaOverTwo * axisX;
					deltaRotationVector[1] = sinThetaOverTwo * axisY;
					deltaRotationVector[2] = sinThetaOverTwo * axisZ;
					deltaRotationVector[3] = cosThetaOverTwo;

					timestamp = event.timestamp;
					float[] rotation = new float[4];
					SensorManager.getQuaternionFromVector(rotation, deltaRotationVector);

					// User code should concatenate the delta rotation we computed with the current rotation
					// in order to get the updated rotation.
					// rotationCurrent = rotationCurrent * deltaRotationMatrix;
					
					concatQuaternion(rotationCurrent,rotation);
					
					int screenRotation = mDisplay.getRotation();
					switch(screenRotation)
					{
						case Surface.ROTATION_0:
							rotation[0] = rotationCurrent[0];
							rotation[1] = rotationCurrent[1];
							rotation[2] = rotationCurrent[2];
							rotation[3] = rotationCurrent[3];
						break;
						case Surface.ROTATION_90:
							rollQuaternionByAngle(rotationCurrent,(float) -(Math.PI/2.f),rotation);
						break;
						case Surface.ROTATION_180:
							rollQuaternionByAngle(rotationCurrent,(float) -Math.PI,rotation);
						break;
						case Surface.ROTATION_270:
							rollQuaternionByAngle(rotationCurrent,(float) -(2.f*Math.PI/3.f),rotation);
						break;
					}
					
					Echo.SetGyroscope(rotation[0],rotation[1],rotation[2],rotation[3]);
					//Echo.SetGyroscope(rotationCurrent[0],rotationCurrent[1],rotationCurrent[2],rotationCurrent[3]);
				}else
				{
					timestamp = event.timestamp;
					Echo.SetGyroscope(1.f,0.f,0.f,0.f);
				}
			}break;
		}
	}

	private static class ContextFactory implements GLSurfaceView.EGLContextFactory
	{
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

		public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig)
		{
			Log.w(TAG, "creating OpenGL ES 1.0 context");
			checkEglError("Before eglCreateContext", egl);
			//int[] attrib_list =
			//{ EGL_CONTEXT_CLIENT_VERSION, 1, EGL10.EGL_NONE };
			EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, null);//attrib_list);
			checkEglError("After eglCreateContext", egl);
			return context;
		}

		public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context)
		{
			egl.eglDestroyContext(display, context);
		}
	}

	private static void checkEglError(String prompt, EGL10 egl)
	{
		int error;
		while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS)
		{
			Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
		}
	}

	private static class ConfigChooser implements
			GLSurfaceView.EGLConfigChooser
	{

		public ConfigChooser(int r, int g, int b, int a, int depth, int stencil)
		{
			mRedSize = r;
			mGreenSize = g;
			mBlueSize = b;
			mAlphaSize = a;
			mDepthSize = depth;
			mStencilSize = stencil;
		}

		/*
		 * This EGL config specification is used to specify 2.0 rendering. We
		 * use a minimum size of 4 bits for red/green/blue, but will perform
		 * actual matching in chooseConfig() below.
		 */
		//private static int EGL_OPENGL_ES2_BIT = 4;
		private static int[] s_configAttribs2 =
				{ 	EGL10.EGL_RED_SIZE, 4,
					EGL10.EGL_GREEN_SIZE, 4,
					EGL10.EGL_BLUE_SIZE, 4,
					//EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
					EGL10.EGL_NONE };

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
		{

			/*
			 * Get the number of minimally matching EGL configurations
			 */
			int[] num_config = new int[1];
			egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

			int numConfigs = num_config[0];

			if (numConfigs <= 0)
			{
				throw new IllegalArgumentException(
						"No configs match configSpec");
			}

			/*
			 * Allocate then read the array of minimally matching EGL configs
			 */
			EGLConfig[] configs = new EGLConfig[numConfigs];
			egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs,
					num_config);

			if (DEBUG)
			{
				printConfigs(egl, display, configs);
			}
			/*
			 * Now return the "best" one
			 */
			return chooseConfig(egl, display, configs);
		}

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
				EGLConfig[] configs)
		{
			for (EGLConfig config : configs)
			{
				int d = findConfigAttrib(egl, display, config,
						EGL10.EGL_DEPTH_SIZE, 0);
				int s = findConfigAttrib(egl, display, config,
						EGL10.EGL_STENCIL_SIZE, 0);

				// We need at least mDepthSize and mStencilSize bits
				if (d < mDepthSize || s < mStencilSize)
					continue;

				// We want an *exact* match for red/green/blue/alpha
				int r = findConfigAttrib(egl, display, config,
						EGL10.EGL_RED_SIZE, 0);
				int g = findConfigAttrib(egl, display, config,
						EGL10.EGL_GREEN_SIZE, 0);
				int b = findConfigAttrib(egl, display, config,
						EGL10.EGL_BLUE_SIZE, 0);
				int a = findConfigAttrib(egl, display, config,
						EGL10.EGL_ALPHA_SIZE, 0);

				if (r == mRedSize && g == mGreenSize && b == mBlueSize
						&& a == mAlphaSize)
					return config;
			}
			return null;
		}

		private int findConfigAttrib(EGL10 egl, EGLDisplay display,
				EGLConfig config, int attribute, int defaultValue)
		{

			if (egl.eglGetConfigAttrib(display, config, attribute, mValue))
			{
				return mValue[0];
			}
			return defaultValue;
		}

		private void printConfigs(EGL10 egl, EGLDisplay display,
				EGLConfig[] configs)
		{
			int numConfigs = configs.length;
			Log.w(TAG, String.format("%d configurations", numConfigs));
			for (int i = 0; i < numConfigs; i++)
			{
				Log.w(TAG, String.format("Configuration %d:\n", i));
				printConfig(egl, display, configs[i]);
			}
		}

		private void printConfig(EGL10 egl, EGLDisplay display, EGLConfig config)
		{
			int[] attributes =
			{ EGL10.EGL_BUFFER_SIZE, EGL10.EGL_ALPHA_SIZE, EGL10.EGL_BLUE_SIZE,
					EGL10.EGL_GREEN_SIZE,
					EGL10.EGL_RED_SIZE,
					EGL10.EGL_DEPTH_SIZE,
					EGL10.EGL_STENCIL_SIZE,
					EGL10.EGL_CONFIG_CAVEAT,
					EGL10.EGL_CONFIG_ID,
					EGL10.EGL_LEVEL,
					EGL10.EGL_MAX_PBUFFER_HEIGHT,
					EGL10.EGL_MAX_PBUFFER_PIXELS,
					EGL10.EGL_MAX_PBUFFER_WIDTH,
					EGL10.EGL_NATIVE_RENDERABLE,
					EGL10.EGL_NATIVE_VISUAL_ID,
					EGL10.EGL_NATIVE_VISUAL_TYPE,
					0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
					EGL10.EGL_SAMPLES,
					EGL10.EGL_SAMPLE_BUFFERS,
					EGL10.EGL_SURFACE_TYPE,
					EGL10.EGL_TRANSPARENT_TYPE,
					EGL10.EGL_TRANSPARENT_RED_VALUE,
					EGL10.EGL_TRANSPARENT_GREEN_VALUE,
					EGL10.EGL_TRANSPARENT_BLUE_VALUE,
					0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,
					0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
					0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,
					0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,
					EGL10.EGL_LUMINANCE_SIZE, EGL10.EGL_ALPHA_MASK_SIZE,
					EGL10.EGL_COLOR_BUFFER_TYPE, EGL10.EGL_RENDERABLE_TYPE,
					0x3042 // EGL10.EGL_CONFORMANT
			};
			String[] names =
			{ "EGL_BUFFER_SIZE", "EGL_ALPHA_SIZE", "EGL_BLUE_SIZE",
					"EGL_GREEN_SIZE", "EGL_RED_SIZE", "EGL_DEPTH_SIZE",
					"EGL_STENCIL_SIZE", "EGL_CONFIG_CAVEAT", "EGL_CONFIG_ID",
					"EGL_LEVEL", "EGL_MAX_PBUFFER_HEIGHT",
					"EGL_MAX_PBUFFER_PIXELS", "EGL_MAX_PBUFFER_WIDTH",
					"EGL_NATIVE_RENDERABLE", "EGL_NATIVE_VISUAL_ID",
					"EGL_NATIVE_VISUAL_TYPE", "EGL_PRESERVED_RESOURCES",
					"EGL_SAMPLES", "EGL_SAMPLE_BUFFERS", "EGL_SURFACE_TYPE",
					"EGL_TRANSPARENT_TYPE", "EGL_TRANSPARENT_RED_VALUE",
					"EGL_TRANSPARENT_GREEN_VALUE",
					"EGL_TRANSPARENT_BLUE_VALUE", "EGL_BIND_TO_TEXTURE_RGB",
					"EGL_BIND_TO_TEXTURE_RGBA", "EGL_MIN_SWAP_INTERVAL",
					"EGL_MAX_SWAP_INTERVAL", "EGL_LUMINANCE_SIZE",
					"EGL_ALPHA_MASK_SIZE", "EGL_COLOR_BUFFER_TYPE",
					"EGL_RENDERABLE_TYPE", "EGL_CONFORMANT" };
			int[] value = new int[1];
			for (int i = 0; i < attributes.length; i++)
			{
				int attribute = attributes[i];
				String name = names[i];
				if (egl.eglGetConfigAttrib(display, config, attribute, value))
				{
					Log.w(TAG, String.format("  %s: %d\n", name, value[0]));
				} else
				{
					// Log.w(TAG, String.format("  %s: failed\n", name));
					while (egl.eglGetError() != EGL10.EGL_SUCCESS)
						;
				}
			}
		}
		
		// Subclasses can adjust these values:
		protected int mRedSize;
		protected int mGreenSize;
		protected int mBlueSize;
		protected int mAlphaSize;
		protected int mDepthSize;
		protected int mStencilSize;
		private int[] mValue = new int[1];
	}

	private static class Renderer implements GLSurfaceView.Renderer
	{
		private AssetManager mAssetManager;
		private String mInternalApplicationDirectory;
		private float mXDPI;
		private float mYDPI;
		private Activity mActivity;
		
		public Renderer(AssetManager assetManager, String internalApplicationDirectory, float screenXDPI, float screenYDPI, Activity activity)
		{
			mAssetManager = assetManager;
			mInternalApplicationDirectory = internalApplicationDirectory;
			mXDPI = screenXDPI;
			mYDPI = screenYDPI;
			mActivity = activity;
		}
		
		public void onDrawFrame(GL10 gl)
		{
			if(!Echo.Update())
			{
				if(mActivity!=null)
				{
					mActivity.finish();
				}
			}
		}

		public void onSurfaceChanged(GL10 gl, int width, int height)
		{
			if(!Echo.IsInitialised())
			{
				Echo.Initialise(width, height, mXDPI, mYDPI, mAssetManager,mInternalApplicationDirectory);
			}else
			{
				
				Echo.GraphicsLost();	//This needs to be moved to a more appropriate place
				Echo.SetResolution(width, height, mXDPI, mYDPI);
				Echo.GraphicsRestored();
			}
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config)
		{
			gl.glClearColor(0,0,0,0);
		}
	}

	@Override public void onPause() {
		super.onPause();
		mSensorManager.unregisterListener(this);
		Echo.Pause();
	}

	@Override public void onResume() {
		super.onResume();
		mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_GAME);
		mSensorManager.registerListener(this, mGravity, SensorManager.SENSOR_DELAY_GAME);
		mSensorManager.registerListener(this, mGyroscope, SensorManager.SENSOR_DELAY_GAME);
		Echo.Resume();
	}

	public void onStart()
	{
		// TODO Auto-generated method stub
		
	}

	public void onRestart()
	{
		// TODO Auto-generated method stub
		//if(!Echo.isInitialised())
		//{
		
		//}
	}

	public void onDestroy()
	{
		Echo.CleanUp();
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy)
	{
		// TODO Auto-generated method stub
		
	}
}
