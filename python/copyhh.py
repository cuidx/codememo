#coding=gbk
import os
import shutil
import re


def copyfiles (source_path,dest_path):
    print source_path
    print dest_path

    for root, dirs, filenames in os.walk(source_path) :
        if root == source_path :
            for name in filenames:
                print name
                print root
                if re.match(ur"(.*)\.h",name) :
                    print source_path + "\\" + name
                    print dest_path + "\\" + name
                    shutil.copy(source_path + "\\" + name, dest_path + "\\" + name)
                

            for path in dirs:
                print path
                if not os.path.exists(dest_path + "\\" + path) :
                    os.mkdir(dest_path + "\\" + path)
                if not re.match(ur"\.(.*)",path) :
                    copyfiles(os.path.join(root,path), dest_path + "\\" + path)
            



copyfiles("C:\\cuidx\\tools\\webrtc2014819(vs2013包含编译好的lib)", "C:\\cuidx\\tools\\webrtclib")
