
// https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Swap_chain
    
"How exactly the queue works and the conditions for presenting an image from the queue 
depend on how the swap chain is set up, but the general purpose of the swap chain is 
to synchronize the presentation of images with the refresh rate of the screen."
...
"Just checking if a swap chain is available is not sufficient, because it may not actually be 
compatible with our window surface. Creating a swap chain also involves a lot more settings 
than instance and device creation, so we need to query for some more details before we're able to proceed.

There are basically three kinds of properties we need to check:
 - Basic surface capabilities (min/max number of images in swap chain, min/max width and height of images)
 - Surface formats (pixel format, color space)
 - Available presentation modes"