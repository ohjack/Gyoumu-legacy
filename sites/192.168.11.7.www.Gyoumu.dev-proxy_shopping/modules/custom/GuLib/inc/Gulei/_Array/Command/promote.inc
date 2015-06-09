<?php
namespace Gulei\_Array\Command;

class promote extends \Gulei\_Array\Command\_Abstract
{
  public function execute($args = null)
  {
    list($key, $DROPKEY, $GROUP) = $args;
    $DROPKEY = ($DROPKEY === null) ? false : $DROPKEY;
    $GROUP = ($GROUP === null) ? false : $GROUP;
    $newArry = array();

    foreach($this->ArryObj as $item){
      if(!isset($item[$key])){
        dpr($item);
        echo '<pre>';
        throw new Exception(__CLASS__ . ': Failed to promote. Key "' . $key . '" doesn\'t exist');
      }

      $ind = (is_array($item[$key])) ? $item[$key][0] : $item[$key];

      if(is_array($item) && $DROPKEY){
        unset($item[$key]);
        if(count($item) == 1){
          $item = array_shift($item);
        }
      }

      if ($GROUP) {
        $newArry[$ind][] = $item;
      }
      else {
        $newArry[$ind] = $item;
      }
    }
    return $newArry;
  }
}