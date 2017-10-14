def doImageChange():
    from PIL import Image
    im = Image.open("C:/Users/cuidx/Desktop/bfq/61000000_gray.jpg")
    pix = im.load()
    w = im.size[0]
    h = im.size[1]
    for x in range(0,w-1) :
            for y in range(0,h-1):
                    if pix[x,y] > 200 :
                            pix[x,y] = 255
                    elif pix[x,y] > 150 :
                            pix[x,y] = 200
                    elif pix[x,y] > 100 :
                            pix[x,y] = 150
                    elif pix[x,y] > 50 :
                            pix[x,y] = 100
                    else :
                        pix[x,y] = 0


    im.save("C:/Users/cuidx/Desktop/bfq/61000000_gray_c.jpg")

def doImageChange1(val):
    from PIL import Image
    im = Image.open("C:/Users/cuidx/Desktop/bfq/61000000_gray.jpg")
    pix = im.load()
    w = im.size[0]
    h = im.size[1]
    for x in range(0,w-1) :
            for y in range(0,h-1):
                    if pix[x,y] > val :
                            pix[x,y] = 255
                    else :
                        pix[x,y] = 0
    filename = "C:/Users/cuidx/Desktop/bfq/61000000_gray_c_" + str(val) + ".jpg"
    im.save(filename)

def doImageChange2(val):
    from PIL import Image
    im = Image.open("C:/Users/cuidx/Desktop/bfq/61000000_gray.jpg")
    pix = im.load()
    w = im.size[0]
    h = im.size[1]
    for x in range(0,w-1) :
            for y in range(0,h-1):
                    if pix[x,y] < val :
                        pix[x,y] = 0
    filename = "C:/Users/cuidx/Desktop/bfq/61000000_gray_c1_" + str(val) + ".jpg"
    im.save(filename)

doImageChange2(50)
doImageChange2(80)
doImageChange2(100)
doImageChange2(150)
doImageChange2(180)
doImageChange2(200)
doImageChange2(225)
