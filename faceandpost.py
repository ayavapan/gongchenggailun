import sensor, time, image, pyb,utime

from pyb import UART,LED
uart = UART(3, 115200, timeout_char=1000)
startpose=bytearray([0xFF,0xFF])
idofpeople=0x00
stoppose=bytearray([0xAA,0xAA])
p = pyb.Pin("P0", pyb.Pin.OUT_PP)
sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.GRAYSCALE
sensor.set_framesize(sensor.B128X128) # or sensor.QQVGA (or others)
sensor.set_windowing((92,112))
sensor.skip_frames(10) # Let new settings take affect.
sensor.skip_frames(time = 5000) #等待5s
NUM_SUBJECTS =1 #图像库中不同人数，一共6人
NUM_SUBJECTS_IMGS = 10 #每人有20张样本图片
face_cascade = image.HaarCascade("frontalface", stage = 25)
img = sensor.snapshot()
d0 = img.find_lbp((0, 0, img.width(), img.height()))
img = None
pmin = 999999
num=0
findpeople=0;
clock = time.clock()
def min(pmin, a, s):
    global num
    if a<pmin:
        pmin=a
        num=s
    return pmin


def panduanface():
    clock.tick()
    img = sensor.snapshot()
    objects = img.find_features(face_cascade, threshold=0.75, scale_factor=1.25)
    for r in objects:
        img.draw_rectangle(r)
    if(objects):
        print("2")
        return 1;
    else:
        print("0")
        return 0;

while(1):
  img = sensor.snapshot()
  objects = img.find_features(face_cascade, threshold=0.95, scale_factor=1.25)
  #print(objects)
  if(objects):
    #img = image.Image("singtown/%s/1.pgm"%(SUB))
    d0 = img.find_lbp((0, 0, img.width(), img.height()))
    for s in range(1, NUM_SUBJECTS+1):
        dist = 0
        for i in range(2, NUM_SUBJECTS_IMGS+1):
            img = image.Image("singtown/s%d/%d.pgm"%(s, i))
            d1 = img.find_lbp((0, 0, img.width(), img.height()))
            #d1为第s文件夹中的第i张图片的lbp特征
            dist += image.match_descriptor(d0, d1)#计算d0 d1即样本图像与被检测人脸的特征差异度。
        #print("Average dist for subject %d: %d"%(s, dist/NUM_SUBJECTS_IMGS))
            pmin = min(pmin, dist/NUM_SUBJECTS_IMGS, s)#特征差异度越小，被检测人脸与此样本更相似更匹配。
        print(pmin)

    print(num) # num为当前最匹配的人的编号。
    if(pmin>750):
        print("陌生人")
        img = None
        pmin = 999999
        idofpeople=0xA0
        num=0
    else:
      #postinformation= bytearray(num)
      #uart.write(postinformation)
      if num==1:
          print("员工1")
          img = None
          pmin = 999999
          idofpeople=0x01
          uart.write(bytearray([0x00,num]))
          num=0
          sensor.skip_frames(time = 500) #等待5s
      if num==2:
          print("员工2")
          img = None
          pmin = 999999
          idofpeople=0x02
          num=0
          sensor.skip_frames(time = 500) #等待5s
      if num==3:
          print("员工3")
          img = None
          pmin = 999999
          idofpeople=0x03
          num=0
          p.low()
          sensor.skip_frames(time = 500) #等待5s
      if num==4:
          print("员工4")
          img = None
          pmin = 999999
          idofpeople=0x04
          num=0
          p.low()
          sensor.skip_frames(time = 500) #等待5s
  else:
    #print("无人")
    idofpeople=0x00
  uart.write(startpose)
  uart.write(bytearray([idofpeople,0xAA]))

