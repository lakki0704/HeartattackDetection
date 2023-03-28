import numpy as np
from flask import Flask, render_template,request
import pickle#Initialize the flask App
app = Flask(__name__)
model = pickle.load(open('model.pkl', 'rb'))

#default page of our web-app
@app.route('/')
def home():
    return render_template('index.html')

@app.route('/predict',methods=['POST'])
def predict():
    #For rendering results on HTML GUI
    int_features = [float(x) for x in request.form.values()]
    final_features = np.array(int_features).reshape(1,-1)
    heart_prediction = model.predict(final_features)
    return render_template('index.html', prediction_text=' Heart attack possibility : {}'.format(percent(heart_prediction)))

def percent(val):
    if(val==1):
        return "YES"
    else:
        return "NO"

if __name__ == "__main__":
    app.run(debug=True)