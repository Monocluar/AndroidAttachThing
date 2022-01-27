// Copyright (c) 2019 Nineva Studios

package com.Monocular.AndroidAttachThing;

import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;

import com.Monocular.AndroidAttachThing.pickers.api.Picker;

public class AndroidAttachThingActivity extends Activity {

	public static final String EXTRAS_PICKER_TYPE = "EXTRAS_PICKER_TYPE";

	private static final int REQ_CODE_INVALID = -1;

	private static final int REQ_CODE_PERMISSIONS = 444;

	private static Status currentStatus = Status.AVAILABLE;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		NinevaLogUtils.logMethodCall("AndroidGoodiesActivity: onCreate");

		if (currentStatus == Status.IN_PROGRESS) {
			return;
		}
		currentStatus = Status.IN_PROGRESS;

		// Screenshot
		if(getIntent().getBooleanExtra(AGScreenShot.EXTRA_SCREENSHOT, false)) {
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
				startActivityForResult(AGScreenShot.projectionManager.createScreenCaptureIntent(), AGScreenShot.CODE_TAKE_SCREENSHOT);
			}
			return;
		}

		// Pickers
		int pickerType = getIntent().getIntExtra(EXTRAS_PICKER_TYPE, REQ_CODE_INVALID);
		NinevaLogUtils.log("Picker type:" + pickerType);
		if (pickerType == REQ_CODE_INVALID) {
			return;
		}

		switch (pickerType) {
			case Picker.PICK_CONTACT:

				break;
			case Picker.PICK_IMAGE_DEVICE:
				int quality = getIntent().getIntExtra(AGImagePicker.EXTRA_QUALITY, 100);
				AGImagePicker.pickImageInternal(getIntent(), this, quality);
				break;
			case Picker.PICK_IMAGE_CAMERA:
				AGImagePicker.pickImageFromCameraInternal(getIntent(), this);
				break;
			case Picker.PICK_AUDIO:

				break;
			case Picker.PICK_VIDEO_DEVICE:

				break;
			case Picker.PICK_VIDEO_CAMERA:

				break;
			case Picker.PICK_FILE:
				AGFilePicker.pickFileInternal(getIntent(), this);
				break;
			case REQ_CODE_PERMISSIONS:

				break;
		}
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
		NinevaLogUtils.logMethodCall("onActivityResult");
		NinevaLogUtils.log("result code:" + resultCode);
		currentStatus = Status.AVAILABLE;

		switch (requestCode) {
			case Picker.PICK_CONTACT:

				break;
			case Picker.PICK_IMAGE_DEVICE:
			case Picker.PICK_IMAGE_CAMERA:
				AGImagePicker.handlePhotoReceived(requestCode, resultCode, intent, this);
				break;
			case Picker.PICK_AUDIO:

				break;
			case Picker.PICK_VIDEO_DEVICE:

				break;
			case Picker.PICK_VIDEO_CAMERA:

				break;
			case Picker.PICK_FILE:
				AGFilePicker.handleFileReceived(requestCode, resultCode, intent, this);
				break;

			case AGScreenShot.CODE_TAKE_SCREENSHOT:
				if(resultCode == Activity.RESULT_OK) {
					AGScreenShot.takeShotInternal(this.getApplicationContext(),intent);
				} else {
					AGImagePicker.onImageError("Permission denied");
				}
				break;
			default:
				finish();
				break;
		}

		finish();
	}

	private enum Status {
		AVAILABLE,
		IN_PROGRESS
	}
}
