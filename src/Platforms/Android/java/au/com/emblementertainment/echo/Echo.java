/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package au.com.emblementertainment.echo;

import android.content.res.AssetManager;

// Wrapper for native library
public class Echo
{
	static
	{
		System.loadLibrary("openal");
		//System.loadLibrary("Echo3");
	}

	/**
	 * @param width
	 *            the current view width
	 * @param height
	 *            the current view height
	 */
	public static native void Initialise(int width, int height, float xdpi, float ydpi, AssetManager assetManager, String internalApplicationDirectory);
	public static native void CleanUp();
	public static native boolean IsInitialised();
	public static native boolean Update();
	public static native void Pause();
	public static native void Resume();
	public static native void GraphicsRestored();
	public static native void GraphicsLost();
	public static native void SetResolution(int width, int height, float xdpi, float ydpi);
	public static native void SetMouse(float x, float y, int pressed);
	public static native void SetLinearAcceleration(float x, float y, float z);
	public static native void SetGyroscope(float w, float x, float y, float z);
	public static native void SetGravity(float x, float y, float z);
	public static native void SetGenericDigitalInput(String name, boolean value);
	//This is just a hack until a proper interface for speech is developer
	public static native void AddSpeechRecognitionPartialResult(String partialResult, float confidence, int resultNumber, int resultsTotal);
	public static native void AddSpeechRecognitionFinalResult(String finalResult, float confidence, int resultNumber, int resultsTotal);
	public static native boolean GetConfigurationBool(String optionName, boolean defaultValue);
}
