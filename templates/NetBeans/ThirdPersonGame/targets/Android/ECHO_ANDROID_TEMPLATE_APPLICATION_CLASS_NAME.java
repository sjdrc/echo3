package ECHO_ANDROID_TEMPLATE_PACKAGE_NAME;

import java.io.File;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Build;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import au.com.emblementertainment.echo.EchoView;
import au.com.emblementertainment.echo.Echo;

public class ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAME extends Activity
{
	EchoView mView;
	AssetManager mAssetManager;
	String mInternalApplicationDirectory;
	
	static
	{
		System.loadLibrary("ECHO_ANDROID_TEMPLATE_APPLICATION_CLASS_NAMEMain");
	}
	
	@Override protected void onCreate(Bundle icicle)
	{
		super.onCreate(icicle);
		
		mAssetManager = getAssets();
		File filesDir = getApplicationContext().getFilesDir();
		mInternalApplicationDirectory = filesDir.getAbsolutePath();
		
		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics);
		
		mView = new EchoView(getApplication(),getWindowManager().getDefaultDisplay(),mAssetManager,mInternalApplicationDirectory,metrics.xdpi,metrics.ydpi,this);
		setContentView(mView);
		hideSystemUI();
	}
	
	private void hideSystemUI()
	{
		if (Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.KITKAT)
		{
			mView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE
										| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
										| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN 
										| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION 
										| View.SYSTEM_UI_FLAG_FULLSCREEN
										| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
		}else
		{
			mView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE
										| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
										| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
										| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
										| View.SYSTEM_UI_FLAG_FULLSCREEN);
		}
	}
	
	@Override protected void onStart()
	{
		super.onStart();
		mView.onStart();
	}
	
	@Override protected void onStop()
	{
		super.onStop();
		mView.onStart();
	}

	@Override protected void onPause()
	{
		super.onPause();
		mView.onPause();
	}

	@Override protected void onResume()
	{
		super.onResume();
		mView.onResume();
	}

	@Override protected void onRestart()
	{
		super.onRestart();
		mView.onRestart();
	}

	@Override protected void onDestroy()
	{
		super.onDestroy();
		mView.onDestroy();
	}
}
