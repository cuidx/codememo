import httplib
import urllib

def sendhttp():
    data = urllib.urlencode({'service':'GetWanIP','params':'{"phoneId":"69600001"}'})
    headers = {"Content-type":"application/x-www-form-urlencoded"}

    conn = httplib.HTTPConnection('10.160.71.11:8000')
    conn.request('POST','/DetectResultReport/callService', data, headers)
    res = conn.getresponse()

    print res.status
    print res.reason
    print res.read()


if __name__ == '__main__':
    sendhttp()
