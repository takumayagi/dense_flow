This is a sample code for extrating dense flow field given a video.  
**Caution: this code is not maintained anymore, please consider using other stable codes unless you understand the code**

# Dependencies
* OpenCV 3.1+
* GPU environment

# Optical flow calculation usage
```
./denseFlow_gpu -f <input file> -i <output path> -b <threshold> -t <method> -d <device_id> -s <step> -m <output_magnitude>
./denseFlow_gpu -f test.avi -i tmp/flow_xym -b 20 -t 2 -d 0 -s 1 -m 0
```
test.avi: input video

# Optical flow method
0: Farneback  
1: TV-L1  
2: Brox  

For Two-Stream CNN, brox is recommended.
In current implementation, we don't use magnitude for recognition.
