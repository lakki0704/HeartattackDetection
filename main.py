import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
# import hvplot.pandas
from scipy import stats
import pickle

data = pd.read_csv("heart.csv")



from sklearn.preprocessing import StandardScaler

s_sc = StandardScaler()
col_to_scale = ['age', 'trestbps', 'chol', 'thalach', 'oldpeak']
data[col_to_scale] = s_sc.fit_transform(data[col_to_scale])


from sklearn.model_selection import train_test_split

X = data.drop('target', axis=1)
y = data.target
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.0001, random_state=42)


from sklearn.linear_model import LogisticRegression
lr_clf = LogisticRegression(solver='liblinear')
lr_clf.fit(X_train.values, y_train)
pred = lr_clf.predict(X_test.values)
# print(pred)

pickle.dump(lr_clf, open('model.pkl','wb'))

#input = age,sex,cp,testbps,chol,fbs,restecg,thlach,exang(0,1),oldpeak,slope,ca,thal
input = [63,1,3,145,233,1,0,150,0,2.3,0,0,1]
input= np.array(input).reshape(1,-1)
pred2=lr_clf.predict(input)
print("prediction :  ",pred2)

#https://www.section.io/engineering-education/deploying-machine-learning-models-using-flask/