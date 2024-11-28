import openai

# OpenAI API 키 설정(암호화 버전)
openai.api_key = 'myAPI'

# 얼굴 색상
rgb_values = []
with open('faceColor.txt', 'r') as file:
    for line in file:
        rgb = line.strip().replace('[', '').replace(']', '').split(',')
        rgb = tuple(map(int, rgb)) 
        rgb_values.append(rgb)

color_prompt = (
    "1.다음은 얼굴에서 가장 밝은 부분과 가장 어두운 부분 RGB 값들입니다:\n" +
    ", ".join([f"RGB{rgb}" for rgb in rgb_values]) +
    "\n이 값을 바탕으로 피부 톤을 분석하고 퍼스널 컬러를 추정해 주세요."
)
# print(color_prompt)

# 얼굴형 랜드마크
landmark_coordinates = []
with open('faceLandmark.txt', "r") as file:
    for line in file:
        x, y = map(int, line.strip()[1:-1].split(", "))
        landmark_coordinates.append((x, y))

face_prompt = (
    "3. 다음은 얼굴에서 추출한 1-17번 랜드마크 좌표입니다:\n" +
    ", ".join([f"({x}, {y})" for x, y in landmark_coordinates]) +
    "\n이 정보를 바탕으로 얼굴형을 분석하고 어울리는 스타일을 추천해주세요."
)
# print(face_prompt)

response = openai.ChatCompletion.create(
    model="gpt-3.5-turbo",  
    messages=[
        {"role": "user", "content": color_prompt},
        {"role": "user", "content": "2. 추정된 퍼스널 컬러에 가장 어울리는 컬러 RGBA값과 어울리지 않는 컬러 RGBA값을 추천해 주세요."},
        {"role": "user", "content": face_prompt}
    ]
)

print(response['choices'][0]['message']['content'])

