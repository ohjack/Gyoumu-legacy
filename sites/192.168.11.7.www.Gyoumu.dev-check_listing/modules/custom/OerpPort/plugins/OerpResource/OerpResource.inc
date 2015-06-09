<?php 

module_load_include('inc', 'GuLib', 'include/GuFileFinder');

class OerpImageHandler
{
  private $root = '/home/jeremy/SmbGallery';
  private $account_name;
  
  function __construct($aname){
    $this->account_name = $aname;
  }
  
  function getMissingPublish(){
    $dirFixed = new GuFileFinder($this->root . '/Fixed');
    $dirAccn = new GuFileFinder(
      $this->root . '/Publish/' . $this->account_name);
    
    $fixed = $dirFixed->pregSearch('@.*/(.*\.jpg)@i', 1);
    $accn = $dirAccn->pregSearch('@.*/(.*\.jpg)@i', 1);
    
    $keyFixed = array_map('strtolower', array_keys($fixed));
    $fixed = array_combine($keyFixed, $fixed);
    $diff = array_diff($keyFixed, array_keys($accn));

    $missing = array();
    foreach($diff as $each){
      $missing[$each] = $fixed[$each];
    }
    return $missing;
  }
  
  function processNewFixed(){
//  get missing
    $missing = $this->getMissingPublish();
    
//  clear batch folder
    $des = $this->root . '/TitosoyBatch';
    exec(sprintf('rm %s/*', $des));

  //  process wm
  $output = '';
  foreach($missing as $name => $path){
      $wm = getcwd() . '/' .
        drupal_get_path('module', 'OerpResource') . '/watermarks/' . $this->account_name . '.png';
  
      $cmd = sprintf('convert -verbose "%s" -resize 800x -gravity Center -compose over "%s" -composite "%s/%s"',
        array_shift($missing), $wm, $des, $name);
        
      $output .= shell_exec($cmd) . '<br/><br/>';

    }
//    $resp .= shell_exec()
    return $output;
  }
}


?>