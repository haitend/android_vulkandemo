package com.vulkan.demo;

import android.view.Surface;

public class VulkanDemo {

    static {
        System.loadLibrary("vulkan");
        System.loadLibrary("vulkan_demo");
    }

    public VulkanDemo() {
    }

    private static native void setSurface(Surface surface);

    public void run(final Surface surface) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                VulkanDemo.setSurface(surface);
            }
        }).start();
    }

}
