<?php
namespace Gulei\Mapper;

class Basic
{
  protected $recs;
  protected $arch;

  public function __construct($recs)
  {
    $this->recs = $recs;
  }

  protected function appendValue(&$recs, $ind, $name, $val, $field)
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
    $this->arch = new \Gulei\Arch\Element($mapping);

    foreach ($this->arch->xp('./field', $this->arch->getArch()) as $field) {
      $name = $field->getAttribute('name');

      if ($this->arch->xp('./field[1]', $field)->length) {
        $inner_mapped = $this->exe($field);

        foreach($this->recs as $ind => $src_rec){
          $this->appendValue($mappedRecs, $ind, $name, $inner_mapped[$ind], $field);
        }
      }
      else {
        foreach ($this->recs as $ind => $src_rec) {
          if($field->getAttribute('eval')){
            $func = create_function('$rec', $field->nodeValue);
            $val = call_user_func($func, $src_rec);
          }
          else{
            $from = $field->getAttribute('from');
            $val = $src_rec[$from];
          }
          $this->appendValue($mappedRecs, $ind, $name, $val, $field);
        }
      }
    }
    return $mappedRecs;
  }
}