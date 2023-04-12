from flask import Flask, request

app = Flask(__name__)

#####################################
# 사료 또는 물 제공 요청 여부
food = False
water = False

# 남은 사료 또는 물 정보 (disconnect, empty, full)
remain_food = "disconnect"
remain_water = "disconnect"

#####################################
# 서버 연결 확인용 기본 경로

@app.route('/')
def home():
   return 'Home'

#####################################
# IoT 기기에서 지속적으로 호출 (약 1초 주기)
# IoT 기기는 현재 사료와 물 양을 JSON으로 전달
# 서버에서는 전달받은 현재 사료와 물 양 저장
# 사료 또는 물 지급 요청이 있을 경우, 'food' 또는 'water' 반환
# 없을 경우, 'wait' 반환
# IoT 기기는 반환값을 통해 동작 결정

@app.route('/wait', methods=['POST'])
def post_wait():
    global food
    global water

    global remain_food
    global remain_water

    if food == True:
        food = False
        return 'food'
    elif water == True:
        water = False
        return 'water'

    if request.method == 'POST':
        receive = request.get_json()

        if receive:
            remain_food = receive.get('remain_food')

            if remain_food == "1":
                remain_food = "empty"
            else:
                remain_food = "full"

            remain_water = receive.get('remain_water')

            if remain_water == "1":
                remain_water = "empty"
            else:
                remain_water = "full"

    print("remain_food : ", remain_food, ", remain_water : ", remain_water)

    return 'wait'

######################################
# 로블록스에서 사료 또는 물 제공 요청
# 서버는 food 또는 water 변수 값을 변경하고 'food' 또는 'water' 반환
# 중복 요청 시 'wait' 반환

@app.route('/food', methods=['GET'])
def get_food():
    global food

    if food == False:
        food = True
        return 'food'
    return 'wait'

@app.route('/water', methods=['GET'])
def get_water():
    global water

    if water == False:
        water = True
        return 'water'
    return 'wait'

#######################################
# 로블록스에서 남은 사료 또는 물 양 요청
# 서버는 남은 사료 또는 물 양 반환

@app.route('/remain_food', methods=['GET'])
def get_remain_food():
    global remain_food

    return remain_food

@app.route('/remain_water', methods=['GET'])
def get_remain_water():
    global remain_water

    return remain_water

#######################################
# 모든 ip, 포트 5000으로 서버 실행
# http://aws퍼블릭ip:5000/경로

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)