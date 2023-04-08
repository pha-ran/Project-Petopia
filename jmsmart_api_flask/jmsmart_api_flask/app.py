from flask import Flask, request

app = Flask(__name__)
waterisOn = False
foodisOn = False
email = ''
foodcnt = 0
watercnt = 0
wateramount = 0
foodamount = 0
network = {
    'ipaddr': '127.0.0.1',
    'port': '5000'
}

data = {
    'a': 1,
    'b': 2,
    'c': 3,
    'd': 4
}

# @app.route('/post', methods=['POST'])
# def receivepost():
#     global waterisOn
#     waterisOn = not waterisOn
#     return '{'+str(waterisOn)+'}'
#
#
# @app.route('/get', methods=['GET'])
# def get():
#     print(waterisOn)
#     return '{ \"isOn\": ' + str(waterisOn).lower() + ' }'



#음식 제공 횟수 가져오기 아두이노 -> 서버
@app.route('/post/foodcount', methods=['POST'])
def foodcount_post():
    global foodcnt
    foodcnt = request.args.get("receive_food_count")
    print(foodcnt)
    return '{'+str(foodcnt)+'}'

#음식 제공 횟수 보내기  아두이노 -> 서버
@app.route('/get/foodcount', methods=['GET'])
def foodcount_get():
    print(foodcnt)
    return '{'+str(foodcnt)+'}'

#물 제공 횟수 가져오기 서버 -> 로블록스
@app.route('/post/watercount', methods=['POST'])
def watercount_post():
    global watercnt
    watercnt = request.args.get("receive_water_count")
    print(watercnt)
    return '{'+str(watercnt)+'}'

#물 제공 횟수 보내기  서버 -> 로블록스
@app.route('/get/watercount', methods=['GET'])
def watercount_get():
    print(watercnt)
    return '{'+str(watercnt)+'}'

#물 제공 로블록스 -> 서버
@app.route('/post/providewater', methods=['POST'])
def providewater_post():
    global waterisOn
    waterisOn = not waterisOn
    return '{' + str(waterisOn) + '}'

#음식 제공 로블록스 -> 서버
@app.route('/post/providefood', methods=['POST'])
def providefood_post():
    global foodisOn
    foodisOn = not foodisOn
    return '{' + str(foodisOn) + '}'

# 물 제공 서버 -> 아두이노
@app.route('/get/providewater', methods=['GET'])
def providewater_get():
    print(waterisOn)
    return '{ \"waterisOn\": ' + str(waterisOn).lower() + ' }'

# 음식 제공 서버 -> 아두이노
@app.route('/get/providefood', methods=['GET'])
def providefood_get():
    print(foodisOn)
    return '{ \"foodisOn\": ' + str(foodisOn).lower() + ' }'


# 물 양 확인 아두이노 -> 서버
@app.route('/post/wateramount', methods=['POST'])
def wateramount_post():
    global wateramount
    wateramount = request.args.get("receive_water_amount")
    print(wateramount)
    return '{'+str(wateramount)+'}'

# 밥 양 확인 아두이노 -> 서버
@app.route('/post/foodamount', methods=['POST'])
def foodamount_post():
    global foodamount
    foodamount = request.args.get("receive_food_amount")
    print(foodamount)
    return '{'+str(foodamount)+'}'

#물 양 확인 get 서버 -> 로블록스
@app.route('/get/wateramount', methods=['GET'])
def wateramount_get():
    print(wateramount)
    return '{'+str(wateramount)+'}'

#밥 양 확인 get 서버 -> 로블록스
@app.route('/get/foodamount', methods=['GET'])
def foodamount_get():
    print(foodamount)
    return '{'+str(foodamount)+'}'


#전체 데이터 가져오기 아두이노 -> 서버
@app.route('/post/alldata', methods=['POST'])
def alldata_post():
    global data
    data = request.get_json()
    print(data)
    return str(data)

#전체 데이터 보내기 서버 -> 로블록스
@app.route('/get/alldata', methods=['GET'])
def alldata_get():
    print(data)
    return str(data)

@app.route('/post/email', methods=['POST'])
def email_post():
    global email
    email = request.args.get("email")
    print(email)
    return '{'+str(email)+'}'
if __name__ == "__main__":
    app.run(host='0.0.0.0', port=network['port'])
