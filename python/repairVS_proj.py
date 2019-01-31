#coding=utf-8

#通过minidom解析xml文件
import xml.dom.minidom as xmldom
import os
import shutil
import re

def RepairVSroject (filename) :
     xmlfilepath = filename
     print("proj file path：" + xmlfilepath)
     # 得到文档对象
     domobj = xmldom.parse(xmlfilepath)
     #print("xmldom.parse:", type(domobj))
     # 得到元素对象
     elementobj = domobj.documentElement
     #print ("domobj.documentElement:", type(elementobj))

     node = elementobj.firstChild
     while node != None :
          if (node.nodeName == "Configurations") :
               cfgnode = node.firstChild
               while cfgnode != None :
                    if (cfgnode.nodeName == "Configuration") :
                         attrname = cfgnode.getAttribute("Name")
                         if (attrname.find("ARMV4",0,len(attrname)) == -1) :
                              #print (attrname)
                              cfgnode = cfgnode.nextSibling
                         else :
                              print ("delete " + attrname)
                              newcfgnode = cfgnode.nextSibling
                              cfgnode.parentNode.removeChild(cfgnode)
                              cfgnode = newcfgnode
                    else :
                          cfgnode = cfgnode.nextSibling
               node = node.nextSibling
          else :
               print (node.nodeName)
               node = node.nextSibling
     
     backname = filename + ".bakproj"
     shutil.copy(filename, backname)
	 
     xmlstr = elementobj.toxml("utf-8")
     ofile = open(filename,"wb")
     ofile.write(('<?xml version="1.0" encoding="Windows-1252"?>').encode('utf-8'))
     ofile.write(xmlstr)
     ofile.close()

def FindAndRepairVSProjects (rootdir) :
     for root, dirs, filenames in os.walk(rootdir) :
          if root == rootdir :
               for name in filenames:
                    if re.match("(.*)\.vcproj", name) :
                         prjname = rootdir + "\\" + name
                         print("RepairVSroject : " + prjname)
                         RepairVSroject(prjname)
               for path in dirs:
                    if not re.match("\.(.*)",path) :
                         FindAndRepairVSProjects(os.path.join(root,path))
   
FindAndRepairVSProjects("C:\\cuidx\\tools\\pjproject-2.8")
   
"""
xmlfilepath = os.path.abspath("libwebrtc.vcproj")
print ("xml文件路径：", xmlfilepath)

# 得到文档对象
domobj = xmldom.parse(xmlfilepath)
print("xmldom.parse:", type(domobj))
# 得到元素对象
elementobj = domobj.documentElement
print ("domobj.documentElement:", type(elementobj))

node = elementobj.firstChild
while node != None :
     if (node.nodeName == "Configurations") :
          cfgnode = node.firstChild
          while cfgnode != None :
               if (cfgnode.nodeName == "Configuration") :
                    attrname = cfgnode.getAttribute("Name")
                    if (attrname.find("ARMV4",0,len(attrname)) == -1) :
                         print (attrname)
                         cfgnode = cfgnode.nextSibling
                    else :
                         print ("delete " + attrname)
                         newcfgnode = cfgnode.nextSibling
                         cfgnode.parentNode.removeChild(cfgnode)
                         cfgnode = newcfgnode
               else :
                     cfgnode = cfgnode.nextSibling
          node = node.nextSibling
     else :
          print (node.nodeName)
          node = node.nextSibling

xmlstr = elementobj.toxml("utf-8")
ofile = open("webrtc.proj.xml","w")
ofile.write(xmlstr)
ofile.close()
#获得子标签
#subElementObj = elementobj.getElementsByTagName("Configuration")
#print ("getElementsByTagName:", type(subElementObj))

#print (len(subElementObj))
# 获得标签属性值
#for i in range(len(subElementObj)):
#     attrname = subElementObj[i].getAttribute("Name")
#     if (attrname.find("ARMV4",0,len(attrname)) == -1) :
#          print (attrname)
#     else :
#          configurations.removeChild(subElementObj[i])


#区分相同标签名的标签
#subElementObj1 = elementobj.getElementsByTagName("caption")
#for i in range(len(subElementObj1)):
#    print ("subElementObj1[i]:", type(subElementObj1[i]))
#    print (subElementObj1[i].firstChild.data)  #显示标签对之间的数据
"""

