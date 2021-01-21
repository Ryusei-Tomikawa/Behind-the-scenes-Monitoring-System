<h1> 成果物 "人間の背後を監視する装置"</h1>

<h1>機能</h1>
1. 歩行中に一定距離範囲に入ることで背後に人がいることを赤外線アレイセンサー(<a href="https://www.switch-science.com/catalog/3848/" rel="noopener noreferrer" target="_blank">https://www.switch-science.com/catalog/3848/</a>)を用いて人検知を行う.<br>
<br>
2. 人検知後,WebServerによる通知,音声出力,カメラによる画像取得,一定時間の時刻取得を行う.


<h1>ファイルの説明</h1>
1. BIN<br>
<li>Audioを使用する際に必要なファイル(SDカードに保存するファイル)<br>
<br>  
2. BackMonitor<br>
2-1 backmonitor1.ino<br>
<li>WebServerによる通知,カメラによる画像取得,一定時間の時刻取得を行うプログラム<br>
 
2-2 backmonitor2.ino<br>
<li>WebServerによる通知,Audioによる音声出力,一定時間の時刻取得を行うプログラム<br>
<br>
3. SOUND<br>
<li>使用した音声データ<br>
また,本音声は<a href="https://otologic.jp/free/se/warning01.html" rel="noopener noreferrer" target="_blank">https://otologic.jp/free/se/warning01.html</a>を用いて作成した


<h1>使用方法</h1>
1. SDカード内にBIN,SOUNDの各ファイルを書き込む<br>
<br>
2. SDカード内にdirファイルを作成し,その中にtime.txtを作成する.<br>
<br>
3. SPRESENSEにMicroSDカードを挿入し,バッテリー電源を接続する.<br>
<br>
4. backmonitor.inoのSSID,PASSは必要に応じて各自変更してもよい(defaultでも動作可能)<br>
→その際のSSID,PASSは,"SprESP8266AP","123123123"である.<br>
<br>
5. backmonitor.inoをSPRESENSEに書き込むと人検知が開始する.<br>
<br>
6. 赤外線アレイセンサーにより人検知した場合,シリアルモニターに出力される "http://"出力されるIPアドレス" にスマホやPCでアクセスするとWebServerにアクセスし通知する.<br>
<br>
7. 6と同時にカメラによる画像取得,一定時間の時刻取得,音声出力を行う.<br>
<br>
8. SPRESENSEに挿入したMicroSDカードには,"PICT000～PICT0009"という名前の画像が保存され,/dir/time.txtには,人検知をした時刻が保存される.<br>
<br>


<h1>SysMLモデル</h1>
<h3>コンテキスト図</h3>
![context](https://user-images.githubusercontent.com/77111746/105327755-a6a61100-5c12-11eb-9684-d736e97a28dd.JPG)
