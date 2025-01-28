//
//  sdl_bgfx_ios.mm
//  Crossplatform
//
//  Created by Charlie on 28/1/2025.
//

#include <bgfx/platform.h>
//#import <UIKit/UIViewController.h>
#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

//class HideStatusBarRootController: UIViewController {
//
//    // We effectively need a way of hiding the status bar for this new window
//    // so we have to set a faux root controller with this preference
//    override func prefersStatusBarHidden() -> Bool {
//        return true;
//    }
//
//};

extern "C" void* CreateMetalLayer(void* window) {
    @autoreleasepool {
        UIWindow* uiWindow = (__bridge UIWindow*) window;
        
        // Create Metal layer
        CAMetalLayer* metalLayer = [CAMetalLayer layer];
        metalLayer.device = MTLCreateSystemDefaultDevice();
        metalLayer.pixelFormat = MTLPixelFormatRGBA8Unorm;
        metalLayer.framebufferOnly = YES;
        
        metalLayer.frame = uiWindow.layer.bounds;
        metalLayer.contentsScale = [UIScreen mainScreen].nativeScale;
        
        [uiWindow.layer addSublayer:metalLayer];
        uiWindow.layer.opaque = YES;
//        uiWindow.rootViewController = HideStatusBarRootController();
        
        return (__bridge void*) metalLayer;
    }
}
