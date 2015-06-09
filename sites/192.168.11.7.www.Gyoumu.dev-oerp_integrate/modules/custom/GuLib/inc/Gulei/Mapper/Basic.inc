<?php
namespace Gulei\Mapper;

class Basic
{
  private $recs;

  public function __construct($recs)
  {
    $this->recs = $recs;
  }

  private function appendValue(&$recs, $ind, $name, $val)
  {
    if($name == '_seq_'){
      $recs[$ind][] = $val;
    }
    else{
      $recs[$ind][$name] = $val;
    }
  }

  public function exe($mapping, $ref = null)
  {
    $mappedRecs = array();
    $arch = new \Gulei\Arch\Element($mapping);

    foreach ($arch->xp('./field', $arch->getArch()) as $field) {
      $name = $field->getAttribute('name');

      if ($arch->xp('./field[1]', $field)->length) {
        $inner_mapped = $this->exe($field);

        foreach($this->recs as $ind => $src_rec){
          $this->appendValue($mappedRecs, $ind, $name, $inner_mapped[$ind]);
        }
      }
      else {
        foreach ($this->recs as $ind => $src_rec) {
          if($field->getAttribute('eval')){
            $val = eval($field->nodeValue);
          }
          else{
            $from = $field->getAttribute('from');
            $val = $src_rec[$from];
          }
          $this->appendValue($mappedRecs, $ind, $name, $val);
        }
      }
    }
    return $mappedRecs;
  }
}