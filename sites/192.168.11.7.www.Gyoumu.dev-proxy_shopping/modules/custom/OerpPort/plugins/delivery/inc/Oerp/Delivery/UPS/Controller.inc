<?php
namespace Oerp\Delivery\UPS;

class Controller
{
  protected $in_path = '/home/jeremy/UpsListener/in';
  protected $log_path = '/home/jeremy/UpsListener/out';
  protected $id;
  protected $rec;

  function __construct($id)
  {
    $this->id = $id;
  }

  function getId()
  {
    return $this->id;
  }

  public function getFilename($ext){
    return $this->in_path . '/' . $this->id . '.' . $ext;
  }

  public function isSent()
  {
    $exts = array('xml', 'xxx', 'Out');

    foreach($exts as $ext){
      if(file_exists($this->getFilename($ext))){
        return true;
      }
    }
    return false;
  }

  public function isProcessed()
  {
    return file_exists($this->getFilename('Out'));
  }

  public function isPrint()
  {
    $out_file = $this->getFilename('Out');

    if(file_exists($out_file) && filesize($out_file) > 0){
      return true;
    }
    return false;
  }

  public function getResponse()
  {
    if(!$this->isPrint())
      return false;

    return file_get_contents($this->getFilename('Out'));
  }

  public function getTrackNum()
  {
    $resp = $this->getResponse();

    if(!$resp)
      return false;

    $ns = array(
      'u' => 'x-schema:OpenShipments.xdr'
    );

    $ArchResp = new \Gulei\Arch\Param\Helper($resp, array('ns' => $ns));
    $nums = array();

    $xp = '/u:OpenShipments/u:OpenShipment/u:ProcessMessage/u:TrackingNumbers/u:TrackingNumber';

    foreach($ArchResp->xp($xp) as $ele){
      $nums[] = $ele->nodeValue;
    }
    return $nums[0];
  }

  public function getOutputDate()
  {
    return date('Ymd', filemtime($this->getFilename('Out')));
  }

  public function getLogMessage()
  {
    if(!$this->isProcessed()){
      return '_PRINT_';
    }

    $logs = glob($this->log_path . '/*.log');
    rsort($logs);

    foreach ($logs as $log) {
      $src = file_get_contents($log);
      $src = iconv('big-5', 'UTF-8', $src);

      $secs = explode('Importing from ', $src);
      $secs = array_reverse($secs);

      foreach($secs as $sec) {
        $grab = '';

        if (strpos($sec, $this->id) === 0) {
          $grab = $sec;
        }

        if ($grab) {
          $pat = "@             \d\..*@u";
          preg_match_all($pat, $sec, $m);

          if ($m[0]) {
            $msg = array_map('trim', $m[0]);
            return implode("\n", $msg);
          }
        }
      }
    }
    return false;
  }

  public function getRecordXml($src, $pkg)
  {
    $Rec = new \Oerp\Util\_Array(array($src));
    $recs = array();
    $Rec->inherit($recs);

    $h = new \Oerp\Delivery\Helper();
    $recs[0]['desc'] = $h->getSkuDesc($src);

    if ($pkg['length']) {
      $recs[0]['length'] = $pkg['length'];
      $recs[0]['width'] = $pkg['width'];
      $recs[0]['height'] = $pkg['height'];

      $wt = $pkg['weight'] / 1000;
      $recs[0]['weight'] = $wt < 0.5 ? 0.5 : $wt;

      $arch = file_get_contents(__DIR__ . '/UpsPrintTemplate_with_dim.xml');
    }
    else {
      $arch = file_get_contents(__DIR__ . '/UpsPrintTemplate.xml');
    }

    $tpl = new \Gulei\Arch\Template($arch, 'o', $recs[0]);
    return $tpl->getArch();
  }

  public function clearRecord()
  {
    $toClear = array('xml', 'xxx', 'Out');

    foreach($toClear as $ext){
      $file = $this->getFilename($ext);

      if(file_exists($file)){
        unlink($file);
      }
    }
  }

  public function sendRecord($rec, $pkg)
  {
    if($this->isSent()){
      $this->clearRecord();
    }

    $xml = $this->getRecordXml($rec, $pkg);
    return file_put_contents($this->getFilename('xml'), $xml);
  }
}