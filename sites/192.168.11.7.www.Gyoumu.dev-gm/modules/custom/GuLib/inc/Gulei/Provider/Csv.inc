<?php
namespace Gulei\Provider;

module_load_include('php', 'GuLib', 'lib/parsecsv.lib');

class Csv implements \Gulei\Provider\_Interface
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