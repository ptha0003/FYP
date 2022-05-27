#!/usr/bin/env python3
import serial
from pymongo import MongoClient
from datetime import datetime
from pytz import timezone
import numpy as np
from collections import defaultdict
import urllib.parse
import time

tz = timezone('Australia/Melbourne')
myDict = dict()
global test
test = []

def UpdateEnterExitTime(db,query,cmd):
    db.update_one(query,cmd)

def AddIntoDetectedDB(userDetails,ID, detectedDB,roomName,RSSI):
    uniqueID = ID + " " + roomName
    alreadyExistInDB = detectedDB.find_one({'uniqueID':uniqueID}) #check if user has entered any rooms yet (i.e. if details are already in detectedDB)
    if(alreadyExistInDB):
        if(alreadyExistInDB['roomName'] == roomName): #if user is already detected and same room name therefore user is leaving room update DB to add in exit time
            if(len(alreadyExistInDB['enterTime']) > len(alreadyExistInDB['exitTime'])):
               UpdateEnterExitTime(detectedDB,{'uniqueID':uniqueID},{"$push": {'exitTime':datetime.now(tz).strftime("%m/%d/%Y, %H:%M:%S")}})
            else:
               UpdateEnterExitTime(detectedDB,{'uniqueID':uniqueID},{"$push": {'enterTime':datetime.now(tz).strftime("%m/%d/%Y, %H:%M:%S")}})              
        else: #otherwise user has entered a new room
            detectedUsersRFID = {'uniqueID':uniqueID,'fName': userDetails['fName'],'lName':userDetails['lName'],'PhoneNum':userDetails['phoneNum'], 'email':userDetails['email'],'roomName':roomName,
                                     'enterTime':[datetime.now(tz).strftime("%m/%d/%Y, %H:%M:%S")],'exitTime':[],'RSSI':RSSI}
            detectedDB.insert_one(detectedUsersRFID)   
    else:
        #print("Add entry into db")
        detectedUsersRFID = {'uniqueID':uniqueID,'fName': userDetails['fName'],'lName':userDetails['lName'],'PhoneNum':userDetails['phoneNum'], 'email':userDetails['email'],'roomName':roomName,
                                     'enterTime':[datetime.now(tz).strftime("%m/%d/%Y, %H:%M:%S")] ,'exitTime':[],'RSSI':RSSI}
        detectedDB.insert_one(detectedUsersRFID)
                 
    #print(alreadyExistInDB)



#2 antenna testing
# def AddIntoDetectedDB2(userDetails,ID, detectedDB,roomName,RSSI,enterOrExit):
#     uniqueID = ID + " " + roomName
#     alreadyExistInDB = detectedDB.find_one({'uniqueID':uniqueID}) #check if user has entered any rooms yet (i.e. if details are already in detectedDB)
#     if(alreadyExistInDB):
#         if(alreadyExistInDB['roomName'] == roomName): #if user is already detected and same room name therefore user is leaving room update DB to add in exit time
#             if(len(alreadyExistInDB['enterTime']) > len(alreadyExistInDB['exitTime'])):
#                UpdateEnterExitTime(detectedDB,{'uniqueID':uniqueID},{"$push": {'exitTime':datetime.now(tz).strftime("%m/%d/%Y, %H:%M:%S")}})
#             else:
#                UpdateEnterExitTime(detectedDB,{'uniqueID':uniqueID},{"$push": {'enterTime':datetime.now(tz).strftime("%m/%d/%Y, %H:%M:%S")}})              
#         else: #otherwise user has entered a new room
#             detectedUsersRFID = {'uniqueID':uniqueID,'fName': userDetails['fName'],'lName':userDetails['lName'],'PhoneNum':userDetails['phoneNum'], 'email':userDetails['email'],'roomName':roomName,
#                                      'enterTime':[datetime.now(tz).strftime("%m/%d/%Y, %H:%M:%S")],'exitTime':[],'RSSI':RSSI}
#             detectedDB.insert_one(detectedUsersRFID)   
#     else:
#         #print("Add entry into db")
#         detectedUsersRFID = {'uniqueID':uniqueID,'fName': userDetails['fName'],'lName':userDetails['lName'],'PhoneNum':userDetails['phoneNum'], 'email':userDetails['email'],'roomName':roomName,
#                                      enterOrExit:[datetime.now(tz).strftime("%m/%d/%Y, %H:%M:%S")] ,'exitTime':[],'RSSI':RSSI}
#         detectedDB.insert_one(detectedUsersRFID)
#                  
#     #print(alreadyExistInDB)

    
    #2 antenna testing
# def clearLists(myDict,key):
#     test.clear()
#     myDict[key]['seq'].clear()
# 
# def f7(seq): #source https://stackoverflow.com/questions/480214/how-do-you-remove-duplicates-from-a-list-whilst-preserving-order
#     seen = set()
#     seen_add = seen.add
#     return [x for x in seq if not (x in seen or seen_add(x))]
# 
# def detectEnteringOrExiting(myDict,detectedDB):
#     #print(myDict)
#     for key in myDict:
#         userDetails = registerUsers.find_one({'RFID': RFID_ID})
#         sequence = myDict[key]['seq']
#         sequence = f7(sequence)
#         #print(sequence)
#         if(len(sequence) == 2):
#             if sequence[0] == '1' and sequence[1] =='2':
#                 print("Entering")
#                 AddIntoDetectedDB2(userDetails,myDict[key]["ID"], detectedDB,myDict[key]["roomName"],1,'enterTime')
#                 clearLists(myDict,key)
#             elif sequence[0] == '2' and sequence[1] == '1':
#                 print("Exiting")
#                 AddIntoDetectedDB2(userDetails,myDict[key]["ID"], detectedDB,myDict[key]["roomName"],1,'exitTime')
#                 clearLists(myDict,key)



if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=0)
    ser.reset_input_buffer()
    username = urllib.parse.quote_plus('xxx') 
    password = urllib.parse.quote_plus('xxx')
    client = MongoClient('mongodb://%s:%s@192.168.0.14:27017' % (username,password))
    db = client.RFIDFYP
    registerUsers = db['registeredusers']
    detectedUsers = db['detectedusers']
    userDetails = registerUsers.find_one({'RFID': '79197031ff'})
    

    startTime = time.time()
    currentTime = 0
    while True:
        currentTime = time.time()
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            #print(line)
            separator1 = line.find("\\")
            separator2 = line.find("/")
            #temp = line[0:separator1] #2 antenna testing
            #roomName = temp[0:temp.find("-")]
            #seqN = temp[temp.find("-")+1:]
            roomName = line[0:separator1]
            if(separator2 != -1): #For UHF system
                RSSI = int(line[separator2+1:])
                RFID_ID = line[separator1+4:separator2]
            else: #for HF system
                RSSI = 0
                RFID_ID = line[separator1+4:]
                        
            #print("roomName = {0}, RFID_ID = {1}, RSSI = {2}".format(roomName,RFID_ID,RSSI))   debugging         
            userDetails = registerUsers.find_one({'RFID': RFID_ID})
            #print(userDetails)
            if(userDetails): #detected a registered user enter if block to start adding details to detected database
                AddIntoDetectedDB(userDetails,RFID_ID,detectedUsers,roomName,RSSI)
                
                
                
                
#             test.append(seqN)
#             if RFID_ID in myDict:
#                 pass
#             else:
#                 myDict[RFID_ID] = {"ID":RFID_ID,"roomName": roomName, "seq":[]}
#             myDict[RFID_ID]['seq'].append(test[0])
#             test.clear()
#             print("roomName = {0}, RFID_ID = {1}, RSSI = {2} seq={3}".format(roomName,RFID_ID,RSSI,seqN))
        
#         if(currentTime - startTime >= 0.5):
#             detectEnteringOrExiting(myDict,detectedUsers)
#             startTime = currentTime
            
        





#references:
#https://www.mongodb.com/blog/post/getting-started-with-python-and-mongodb
#https://docs.python.org/3/library/datetime.html
#https://create.arduino.cc/projecthub/ansh2919/serial-communication-between-python-and-arduino-e7cce0
        
        

            
        
            




                     
