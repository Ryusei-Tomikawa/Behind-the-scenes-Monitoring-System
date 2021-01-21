<h2> 成果物 "人間の背後を監視する装置"</h2>

<h2>機能</h2>
1.歩行中に一定距離範囲に入ることで背後に人がいることをWebServer,音声で通知してくれる.<br>
2.人検知後,カメラによる画像取得,一定時間の時刻取得を行う.


<h2>ファイルの説明</h2>
1.BIN<br>
<li>Audioを使用する際に必要なファイル(SDカードに保存するファイル)<br>
<br>  
2.BackMonitor<br>
<li>人間の背後を監視するためのArduino用のファイル<br>
<br>
3.SOUND<br>
<li>使用した音声データ<br>
また,本音声は<a href="https://otologic.jp/free/se/warning01.html" rel="noopener noreferrer" target="_blank">https://otologic.jp/free/se/warning01.html</a>を用いて作成した


<h2>使用方法</h2>
1.SDカード内にBIN,SOUNDの各ファイルを書き込む<br>
<br>
2.SDカード内にdirファイルを作成し,その中にtime.txtを作成し保存する.<br>
<br>
3.SPRESENSEにMicroSDカードを挿入し,バッテリー電源を接続する.<br>
<br>
4.backmonitor.inoのSSID,PASSは必要に応じて各自変更してもよい(defaultでも動作可能)<br>
→その際のSSID,PASSは,"SprESP8266AP","123123123"である.<br>
<br>
5.backmonitor.inoをSPRESENSEに書き込むと人検知が開始する.<br>
<br>
6.サーモグラフィにより人検知した場合,シリアルモニターに出力される "http://"出力されるIPアドレス" にスマホやPCでアクセスするとWebServerにアクセスし通知する.<br>
<br>
7.6と同時にカメラによる画像取得,一定時間の時刻取得,音声出力を行う.<br>


<h2>SysMLモデル</h2>

