import requests
import base64
import json
import time
# 这里需要手动输入设备地址，由于使用的是手机的局域网，设备IP地址会改变，无法每次都定位到对应设备
IP1 = 'http://192.168.43.170'
IP2 = 'http://192.168.43.45'

picture_num = 0

# 车牌识别，利用的是百度提供的车牌识别的api
def scan_picture(filename):
    request_url = "https://aip.baidubce.com/rest/2.0/ocr/v1/license_plate"
    # 二进制方式打开图片文件
    f = open(filename, 'rb')
    img = base64.b64encode(f.read())

    params = {"image":img}

    # access_token可能会过期，需要更换
    access_token = '24.dc35bfd2baf04677e417a301aaf25dc1.2592000.1726361761.282335-106856312'
    request_url = request_url + "?access_token=" + access_token
    headers = {'content-type': 'application/x-www-form-urlencoded'}
    
    # 使用requests库发送请求
    response = requests.post(request_url, data=params, headers=headers)
    if list(response.json().keys())[0] == "log_id":
        return "fail"
    if response:
        # 打印车牌信息
        print(response.json()['words_result']['number'])
        return response.json()['words_result']['number']


def recieve_picture():
    url = IP1
    # 向摄像头请求照片
    response = requests.get(url)
    # 保存相片
    if(response.status_code == 200):
        with open(str(picture_num)+ '.jpg', "wb") as f:
            f.write(response.content)

# 向遥感和oled屏幕发送信息
def oled_send(text):
    url = IP2
    data1 = {"car_number" : text, 'right' : right}
    data1 = json.dumps(data1)
    data = {'data':data1}
    response = requests.post(url, data=data)

# 鉴权函数
def right_test(text):
    with open('car_number.json', 'rb') as f:
        json_text = f.read()
    list_text = json.loads(json_text)
    if text in list_text:
        right = 1
    else:
        right = 0
    return right

# while():
recieve_picture()
text = scan_picture(str(picture_num)+ '.jpg')
if text != "fail":
    right = right_test(text)
    oled_send(text)
    # 每10秒发起一次请求
    # time.sleep(10)
