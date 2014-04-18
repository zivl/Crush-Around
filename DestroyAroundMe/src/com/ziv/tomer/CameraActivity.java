package com.ziv.tomer;

import java.io.File;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.NativeCameraView;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.provider.MediaStore.Images;
import android.annotation.SuppressLint;
import android.content.ContentValues;
import android.content.Intent;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;


public class CameraActivity extends FragmentActivity implements
		CvCameraViewListener2 {
	// A tag for log output.
	private static final String TAG = "CameraActivity";
	// A key for storing the index of the active camera.
	private static final String STATE_CAMERA_INDEX = "cameraIndex";
	// The index of the active camera.
	private int cameraIndex;
	// Whether the active camera is front-facing.
	// If so, the camera view should be mirrored.
	private boolean isCameraFrontFacing;
	// The number of cameras on the device.
	private int numOfCameras;
	// The camera view.
	private CameraBridgeViewBase cameraView;
	// Whether the next camera frame should be saved as a photo.
	private boolean isPhotoPending;
	// A matrix that is used when saving photos.
	private Mat imageToSave;

	// Whether an asynchronous menu action is in progress.
	// If so, menu interaction should be disabled.
	private boolean mIsMenuLocked;
	// The OpenCV loader callback.
	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(final int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS:
				Log.d(TAG, "OpenCV loaded successfully");
				cameraView.enableView();
				imageToSave = new Mat();
				break;
			default:
				super.onManagerConnected(status);
				break;
			}
		}
	};

	@SuppressLint("NewApi")
	@Override
	protected void onCreate(final Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		final Window window = getWindow();
		window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		if (savedInstanceState != null) {
			cameraIndex = savedInstanceState.getInt(STATE_CAMERA_INDEX, 0);
		} else {
			cameraIndex = 0;
		}
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD) {
			CameraInfo cameraInfo = new CameraInfo();
			Camera.getCameraInfo(cameraIndex, cameraInfo);
			isCameraFrontFacing = (cameraInfo.facing == CameraInfo.CAMERA_FACING_FRONT);

			numOfCameras = Camera.getNumberOfCameras();
		} else { // pre-Gingerbread
			// Assume there is only 1 camera and it is rear-facing.
			isCameraFrontFacing = false;
			numOfCameras = 1;
		}
		cameraView = new NativeCameraView(this, cameraIndex);
		cameraView.setCvCameraViewListener(this);
		setContentView(cameraView);
				
	}

	@Override
	public void onSaveInstanceState(Bundle savedInstanceState) {
		// Save the current camera index.
		savedInstanceState.putInt(STATE_CAMERA_INDEX, cameraIndex);
		super.onSaveInstanceState(savedInstanceState);
	}

	@Override
	public void onPause() {
		if (cameraView != null) {
			cameraView.disableView();
		}
		super.onPause();
	}

	@Override
	public void onResume() {
		super.onResume();
		OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_8, this,
				mLoaderCallback);
		mIsMenuLocked = false;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		if (cameraView != null) {
			cameraView.disableView();
		}
	}

	@Override
	public boolean onCreateOptionsMenu(final Menu menu) {
		getMenuInflater().inflate(R.menu.activity_camera, menu);
		if (numOfCameras < 2) {
			// Remove the option to switch cameras, since there is
			// only 1.
			menu.removeItem(R.id.menu_next_camera);
		}
		return true;
	}

	@SuppressLint("NewApi")
	@Override
	public boolean onOptionsItemSelected(final MenuItem item) {
		if (mIsMenuLocked) {
			return true;
		}
		switch (item.getItemId()) {
		case R.id.menu_next_camera:
			mIsMenuLocked = true;
			// With another camera index, recreate the activity.
			cameraIndex++;
			if (cameraIndex == numOfCameras) {
				cameraIndex = 0;
			}
			recreate();
			return true;
		case R.id.menu_take_photo:
			mIsMenuLocked = true;
			// Next frame, take the photo.
			isPhotoPending = true;
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	@Override
	public void onCameraViewStarted(final int width, final int height) {
	}

	@Override
	public void onCameraViewStopped() {
	}

	@Override
	public Mat onCameraFrame(final CvCameraViewFrame inputFrame) {
		final Mat rgba = inputFrame.rgba();
		if (isPhotoPending) {
			isPhotoPending = false;
			takePhoto(rgba);
		}
		if (isCameraFrontFacing) {
			// Mirror (horizontally flip) the preview.
			Core.flip(rgba, rgba, 1);
		}
		Mat res = performCanny(rgba);
		return res;
	}
	
	private Mat performCanny(Mat inputFrame){
		Mat output = new Mat();
		Imgproc.Canny(inputFrame, output, 50, 100);
		return output;
	}
	
	private void takePhoto(final Mat rgba) {
		// Determine the path and metadata for the photo.
		final long currentTimeMillis = System.currentTimeMillis();
		final String appName = getString(R.string.app_name);
		final String galleryPath = Environment
				.getExternalStoragePublicDirectory(
						Environment.DIRECTORY_PICTURES).toString();
		final String albumPath = galleryPath + "/" + appName;
		final String photoPath = albumPath + "/" + currentTimeMillis + ".png";
		final ContentValues values = new ContentValues();
		values.put(MediaStore.MediaColumns.DATA, photoPath);
		values.put(Images.Media.MIME_TYPE, LabActivity.PHOTO_MIME_TYPE);
		values.put(Images.Media.TITLE, appName);
		values.put(Images.Media.DESCRIPTION, appName);
		values.put(Images.Media.DATE_TAKEN, currentTimeMillis);
		// Ensure that the album directory exists.
		File album = new File(albumPath);
		if (!album.isDirectory() && !album.mkdirs()) {
			Log.e(TAG, "Failed to create album directory at " + albumPath);
			onTakePhotoFailed();
			return;
		}
		// Try to create the photo.
		Imgproc.cvtColor(rgba, imageToSave, Imgproc.COLOR_RGBA2BGR, 3);
		if (!Highgui.imwrite(photoPath, imageToSave)) {
			Log.e(TAG, "Failed to save photo to " + photoPath);
			onTakePhotoFailed();
		}
		Log.d(TAG, "Photo saved successfully to " + photoPath);
		// Try to insert the photo into the MediaStore.
		Uri uri;

		try {
			uri = getContentResolver().insert(
					Images.Media.EXTERNAL_CONTENT_URI, values);
		} catch (final Exception e) {
			Log.e(TAG, "Failed to insert photo into MediaStore");
			e.printStackTrace();
			// Since the insertion failed, delete the photo.
			File photo = new File(photoPath);
			if (!photo.delete()) {
				Log.e(TAG, "Failed to delete non-inserted photo");
			}
			onTakePhotoFailed();
			return;
		}
		// Open the photo in LabActivity.
		final Intent intent = new Intent(this, LabActivity.class);
		intent.putExtra(LabActivity.EXTRA_PHOTO_URI, uri);
		intent.putExtra(LabActivity.EXTRA_PHOTO_DATA_PATH, photoPath);
		startActivity(intent);
	}

	private void onTakePhotoFailed() {
		mIsMenuLocked = false;
		// Show an error message.
		final String errorMessage = getString(R.string.photo_error_message);
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				Toast.makeText(CameraActivity.this, errorMessage,
						Toast.LENGTH_SHORT).show();
			}
		});
	}
}
