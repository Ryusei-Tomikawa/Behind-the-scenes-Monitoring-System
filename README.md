# 成果物 "人間の背後を監視する装置"

# 機能
1. 歩行中に一定距離範囲に入ることで背後に人がいることを赤外線アレイセンサー(<a href="https://www.switch-science.com/catalog/3848/" rel="noopener noreferrer" target="_blank">https://www.switch-science.com/catalog/3848/</a>)を用いて人検知を行う.　　
　　
2. 人検知後,WebServerによる通知,音声出力,カメラによる画像取得,一定時間の時刻取得を行う.


# ファイルの説明
1. BIN  
・Audioを使用する際に必要なファイル(SDカードに保存するファイル)  
  
2. BackMonitor  
2-1 backmonitor1.ino  
・WebServerによる通知,カメラによる画像取得,一定時間の時刻取得を行うプログラム  
2-2 backmonitor2.ino  
・WebServerによる通知,Audioによる音声出力,一定時間の時刻取得を行うプログラム  

3. SOUND  
・使用した音声データ(Warning.mp3)  
また,本音声は<a href="https://otologic.jp/free/se/warning01.html" rel="noopener noreferrer" target="_blank">https://otologic.jp/free/se/warning01.html</a>を用いて作成した.  

# 使用方法
1. SDカード内にBIN,SOUNDの各ファイルを保存する.  
  
2. SDカード内にdirファイルを作成し,その中にtime.txtを作成する.  

3. SPRESENSEにMicroSDカードを挿入し,バッテリー電源を接続する.  

4. backmonitor.inoのSSID,PASSは必要に応じて各自変更してもよい.(defaultでも動作可能)  
→その際のSSID,PASSは,"SprESP8266AP","123123123"である.  
  
5. backmonitor.inoをSPRESENSEに書き込むと人検知が開始する.  
  
6. 赤外線アレイセンサーにより人検知した場合,シリアルモニターに出力される "http://"出力されるIPアドレス" にスマホやPCでアクセスするとWebServerにアクセスし通知する.  
  
7. 6と同時にカメラによる画像取得,一定時間の時刻取得,音声出力を行う.  
  
8. SPRESENSEに挿入したMicroSDカードには,"PICT000～PICT0009"という名前の画像が保存され,/dir/time.txtには,人検知をした時刻が保存される.  
  

# WebServer通知画面例
![WebServer](https://user-images.githubusercontent.com/77111746/105331728-2209c180-5c17-11eb-8b40-dea29e1e20e5.jpg)  
# /dir/time.txtの例
![time](https://user-images.githubusercontent.com/77111746/105329670-be7e9480-5c14-11eb-85f6-80b2c56cd32e.JPG)  

# デモ動画
## Audio Version
[![Watch the video](https://user-images.githubusercontent.com/77111746/105334628-73678000-5c1a-11eb-9627-ec42715c537c.JPG)](https://user-images.githubusercontent.com/77111746/105333653-52525f80-5c19-11eb-8fc8-de8bb3a48500.mp4) 

# SysMLモデル
## コンテキスト図  
![ユースケース図](https://user-images.githubusercontent.com/77111746/105330017-2634df80-5c15-11eb-9008-61c6f3ed8344.JPG)  

## 要求図  
![要求図](https://user-images.githubusercontent.com/77111746/105332124-9b091900-5c17-11eb-9e15-333ce2340550.JPG)  

## アクティビティ図  
![アクティビティ図](https://user-images.githubusercontent.com/77111746/105332159-a3f9ea80-5c17-11eb-9ee7-62e0e0838993.png)  

## ユースケース図  
![ユースケース図](https://user-images.githubusercontent.com/77111746/105332183-abb98f00-5c17-11eb-8fc7-59982431f9aa.JPG)  

## ブロック定義図  
![ブロック定義図](https://user-images.githubusercontent.com/77111746/105332255-becc5f00-5c17-11eb-8919-4da2e4c75d49.JPG)

