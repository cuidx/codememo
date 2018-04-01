import os

xmlstr = '<?xml version="1.0" encoding="gb2312" ?>\n<addbook>\n'
filename = 'c:/test.txt'
fp = open(filename)
try:
    for line in fp:
        #print line,
        line = str.rstrip(line,'\r\n')
        items = str.split(line,'\t')
       # print items,
        xmlstr += str.format('\t<userinfo>\n\t\t<phoneid>{0}</phoneid>\n\t\t<name>{1}</name>\n\t</userinfo>\n',items[0],items[1])
finally:
    fp.close()

xmlstr += '</addbook>'
print xmlstr,

fw = open('c:/cuidx/text.xml','w')
try:
    fw.write(unicode(xmlstr,'utf8').encode('gbk'))
finally:
    fw.close()
    
