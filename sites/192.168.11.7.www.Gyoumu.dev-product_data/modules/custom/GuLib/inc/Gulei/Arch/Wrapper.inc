<?php
namespace Gulei\Arch;

class Wrapper
{
  protected  $col;
  protected  $rows = array(array());
  protected  $suffix = '';

  function __construct($col){
    $this->col = $col;
  }

  private function c(){
    return count($this->rows);
  }

  public function getSpanned(){
    $spanned = 0;

    foreach($this->rows[$this->c() - 1] as $cell){
      $spanned += (int)$cell['colspan'];
    }
    return $spanned;
  }

  public function addSuffix($src){
    $this->suffix .= $src;
  }

  public function addBreak(){
    $spanned = $this->getSpanned();

    if($spanned > 0 && $spanned < $this->col){
      $this->rows[$this->c() - 1][] = array(
        'data' => '',
        'colspan' => $this->col - $this->getSpanned(),
        'class' => 'gu-break',
      );
    }
    $this->rows[] = array();
  }

  public function addItem($val, $opts = null){
    if(is_null($opts)){
      $opts = array('colspan' => 1);
    }

    if($opts['colspan'] > $this->col) return false;

    $spanned = $this->getSpanned();

    if($opts['colspan'] > $this->col - $spanned){
      $this->addBreak();
    }

    $this->rows[$this->c() - 1][] = array(
      'data' => sprintf('<div class="inner %s">%s</div>',
                        $opts['class'], $val),
      'colspan' => $opts['colspan'],
    );
    return true;
  }

  public function render(){
    $this->addBreak();

    foreach($this->rows as &$row){
      foreach($row as &$cell){
        $width = (float)$cell['colspan'] / $this->col * 100;
        $cell['style'] = 'width: ' . $width . '%;';

        if($width == '100'){
          $cell['class'] = 'single-col';
        }
      }
    }

    $src = theme_table(null, $this->rows, array(
      'class' => 'gu-arch',
    ));

    return $src . $this->suffix;
  }
}