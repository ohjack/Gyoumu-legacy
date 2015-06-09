<?php
namespace Ups;

class Printer
{
  protected $in_path;
  protected $log_path;
  protected $id;
  protected $rec;

  function __construct($in_path, $log_path, $id)
  {
    $this->id = $id;
    $this->in_path = $in_path;
    $this->log_path = $log_path;
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
    return $nums;
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

  public function getRecordXml($rec)
  {
    $arch = file_get_contents('sites/all/modules/custom/UpsPrint/UpsPrintTemplate.xml');
    $tpl = new \Gulei\Arch\Template($arch, 'o', $rec);
    return $tpl->getArch();
  }

  protected function clearRecord()
  {
    $toClear = array('xml', 'xxx', 'Out');

    foreach($toClear as $ext){
      $file = $this->getFilename($ext);

      if(file_exists($file)){
        unlink($file);
      }
    }
  }

  public function sendRecord($rec)
  {
    if($this->isSent()){
      $this->clearRecord();
    }
    return file_put_contents($this->getFilename('xml'), $this->getRecordXml($rec));
  }
}