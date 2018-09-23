package com.vulkan.demo;

import android.app.Activity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MainActivity extends Activity implements SurfaceHolder.Callback {

    private VulkanDemo m_VulkanDemo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        m_VulkanDemo = new VulkanDemo();

        final SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surface);
        surfaceView.getHolder().addCallback(this);
    }
    @Override
    public void surfaceCreated(final SurfaceHolder holder) {
        m_VulkanDemo.run(holder.getSurface());
    }

    @Override
    public void surfaceChanged(final SurfaceHolder holder, final int format, final int width,
                               final int height) {

    }

    @Override
    public void surfaceDestroyed(final SurfaceHolder holder) {
    }
}
