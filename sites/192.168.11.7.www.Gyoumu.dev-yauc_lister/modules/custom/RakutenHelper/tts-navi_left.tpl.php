<style>

#tts_tab{
  width: 186px;
}

#tts_tab-search{
  height: 116px;
  background-image: url("http://image.rakuten.co.jp/titosoy/cabinet/chrome/tab-search.jpg");
}

#tts_left-search{
  position: relative;
  top: 47px;
  left: 5px;
}

#tts_left-search input.text{
  width: 170px;
}

#tts_tab-body{
  padding: 0 10px;
  background-image: url("http://image.rakuten.co.jp/titosoy/cabinet/chrome/tab-bg.jpg");
}

#tts_tab-body div.c {
  font-size: 13px;
  padding-bottom: 0.5em;
}

#tts_tab-body div.c a {
  color: #339;
}

<?php for($lv = 0; $lv <= 10; $lv++): ?>
div.c.lv-<?php echo $lv; ?> {
  padding-left: <?php echo $lv * 10 ?>px;
}
div.c.lv-<?php echo $lv; ?> a {
 font-size: <?php echo (100 - $lv * 10); ?>%;
}
<?php endfor; ?>

#tts_tab-body div.c.lv-0 {
  padding-top: 1.5em;
}

#tts_tab-body div.c.lv-0 a {
  font-weight: bold;
  color: white;
}

#tts_tab-body div.c.lv-1 a {
  font-weight: bold;
  color: #006;
}

#tts_tab-body div.c a {
  text-decoration: none;
}

#tts_tab-body div.c a:hover {
  text-decoration: underline;
}


#tts_tab-body a img{
  border-style: none;
}

#tts_tab-body ul.inner, #tts_tab-body ul.inner-item{
  margin: 0;
  padding: 5px 0;
  list-style-type: none;
}

#tts_tab-body ul.inner li{
  list-style-type: none;
  text-align: right;
  position: relative;
  right: 10px;
  padding: 5px 0;
}

#tts_tab-body ul.inner-item li{
  right: -6px;
  padding: 0;
  margin: 0;
}

#tts_tab-contact{
  height: 128px;
  background-image: url("http://image.rakuten.co.jp/titosoy/cabinet/chrome/tab-contact.jpg");
}

#tts_tab-contact a {
  position: relative;
  top: 80px;
  left: 12px;
  font-size: 12px;
  color: #000;
}

</style>
</head>
<body>
<div id="tts_tab">
  <div id="tts_tab-search">
    <!-- searchall -->
    <form method="get" action="http://esearch.rakuten.co.jp/rms/sd/esearch/vc" id="tts_left-search">
    <input type="hidden" name="sv" value="6">
    <input type="hidden" name="sid" value="264316">
    <input type="hidden" name="su" value="titosoy">
    <input type="hidden" name="sn" value="TitoSoy">
    <input type="text" name="sitem" class="text">
    <input type="hidden" name="f" value="A">
    <input type="submit" value="検索">
    </form>
  </div>
  <div id="tts_tab-body">
    <?php foreach($items as $item): ?>
      <div class="c lv-<?php echo $item['lv']; ?>">
        <a href="<?php echo $item['href']; ?>"><?php echo $item['name']; ?></a>
      </div>
    <?php endforeach; ?>
  </div>
  <div id="tts_tab-contact">
    <a href="mailto:exgoods@maolung.com">exgoods@maolung.com</a>
  </div>
</div>
