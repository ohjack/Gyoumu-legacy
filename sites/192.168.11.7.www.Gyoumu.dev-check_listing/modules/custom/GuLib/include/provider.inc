<?php
namespace Gulei;

require_once __DIR__ . '/../lib/parsecsv.lib.php';

interface InterfaceProvider
{
  public function getData();
  public function getReference();
  public function count();
}

class ProviderLambda implements InterfaceProvider
{
  private $data;

  function __construct($data)
  {
    $this->data = $data;
  }

  public function getData()
  {
    return $this->data;
  }

  public function getReference()
  {
    return array('_data' => $this->data);
  }

  public function count()
  {
    return count($this->data);
  }
}

class ProviderArch implements InterfaceProvider
{
  private $Arch;

  function __construct($arch)
  {
    $this->Arch = new \GuArchElement($arch);
  }

  public function getData()
  {
    return array($this->Arch->extractAttr('string', 'name'));
  }

  public function getReference()
  {
    return array('_arch' => $this->Arch);
  }

  public function count()
  {
    return $this->Arch->xp('//subject')->length;
  }
}

class ProviderCompose implements InterfaceProvider
{
  private $recs;
  private $hds;

  function __construct($recs, $hds)
  {
    $this->recs = $recs;
    $this->hds = $hds;
  }

  public function getData()
  {
    $recs = array();
    foreach($this->recs as $refRec){
      if(empty($refRec)){
        $recs[] = array();
        continue;
      }

      foreach(array_keys($this->hds) as $hdName){
        if(isset($refRec[$hdName]))
          $rec[$hdName] = $refRec[$hdName];

        else
          $rec[$hdName] = null;
      }
      $recs[] = $rec;
    }
    return $recs;
  }

  public function getReference()
  {
    return array('_recs' => $this->recs, '_hds' => $this->hds);
  }

  public function count()
  {
    return count($this->recs);
  }
}

class ProviderCsv implements InterfaceProvider
{
  private $fp;

  public function __construct($fp)
  {
    $this->fp = $fp;
  }

  public function getData()
  {
    $csv = new \parseCSV($this->fp);
    return $csv->data;
  }

  public function count()
  {
    return count($this->getData());
  }

  public function getReference()
  {
    return array('_csv_filepath' => $this->fp);
  }
}