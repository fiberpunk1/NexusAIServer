import requests

# upload image and return the json result
files = {'media': open('test.jpg','rb')}
r = requests.post('http://127.0.0.1:8002/upload',files=files)

print(r.text)

# get the result.jpg and save
image = requests.get('http://127.0.0.1:8002/result.jpg')
with open('result.jpg', 'wb') as img_file:
    img_file.write(image.content)


