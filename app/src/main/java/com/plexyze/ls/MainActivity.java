package com.plexyze.ls;

import androidx.appcompat.app.AppCompatActivity;


import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {

    GLES3JNIView mView;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new GLES3JNIView(getApplication());
        setContentView(mView);
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }
}