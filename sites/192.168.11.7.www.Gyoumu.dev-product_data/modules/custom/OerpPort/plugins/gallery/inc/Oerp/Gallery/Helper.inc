<?php
namespace Oerp\Gallery;

class Helper
{
  public function getRoot()
  {
    return drupal_get_path('module', 'oerp_gallery') . '/gallery';
  }

  public function isSlotName($name){
    return strpos($name, 'pub_') === 0;
  }

  public function extractSlotSku($name) {
    preg_match('/^(del-)?pub_(.*)(-\d*)/', $name, $m);
    return $m[2];
  }

  public function extractSlotFn($name){
    preg_match('/^(del-)?pub_(.*)/', $name, $m);
    return $m[2];
  }

  public function getProcessingDirs()
  {
    $ff = new \Oerp\Gallery\File\Finder\Gallery('input');

    $dirs = array();
    foreach($ff->getList('*', false, GLOB_ONLYDIR) as $line){
      preg_match('@.*/(.{8})(.*)@', $line, $m);
      $dirs[$m[1]] = $m[1] . $m[2];
    }
    return $dirs;
  }

  private function getProcessingOutput()
  {
    $dirs = $this->getProcessingDirs();

    if(!$dirs){
      return 'N/A';
    }

    $skus = array_keys($dirs);
    $q = new \Oerp\Query\Basic('product.product', array(array('default_code', 'in', $skus)), array('default_code', 'name'));
    $prods = $q->raw()->promote('default_code');

    foreach($dirs as $sku => &$dir){
      if($prods[$sku]){
        $dir .= ' [' . $prods[$sku]['name'] . ']';
      }
    }
    $src = implode("\n", $dirs);
    return $src;
  }

  public function getQueuedDirs()
  {
    $queued_src = file_get_contents($this->getRoot() . '/toPhoto');
    return \Gulei\Text\Helper::explode($queued_src);
  }

  public function getInputStatus()
  {
    $procQueued = $this->getQueuedDirs();
    $queued_src = $procQueued ?  implode("\n", $this->getQueuedDirs()) : 'N/A';

    return sprintf(
      '<div><strong>Processing:</strong><pre>%s</pre><strong>Queued:</strong><pre>%s</pre></div>',
      $this->getProcessingOutput(), $queued_src
    );
  }

  public function getPhotoSubmission()
  {
    $h = new \Oerp\Gallery\Helper();
    $cmd = sprintf("find ./%s -mindepth 1 -maxdepth 1 -not -empty -type d",
      $h->getRoot() . '/input'
    );

    $processingDirs = array();
    exec($cmd, $processingDirs);

    $processingDirs = implode("\n", $processingDirs);
    $processingDirs = preg_replace('@.*/(.*)@', "\\1", $processingDirs);
    $processingDirs = \Gulei\Text\Helper::explode($processingDirs);
    $proc_src = $processingDirs ?  implode('<br/>', $processingDirs) : 'N/A';

    $queuedDirs = $h->getQueuedDirs();
    $que_src = $queuedDirs ?  implode('<br/>', $queuedDirs) : 'N/A';

    $output = '';
    $output .= sprintf('<h3>Submitting photos:</h3><div>%s</div>', $proc_src);
    $output .= sprintf('<h3>Creating folders:</h3><div>%s</div>', $que_src);

    if(!$processingDirs && !$queuedDirs){
      $isSubmitable = false;
      $output = 'Nothing to submit.';
    }
    else{
      $isSubmitable = true;
    }

    return (object)array(
      'msg' => $output,
      'isSubmitable' => $isSubmitable
    );
  }

  public function submitPhoto()
  {
    $cmd = sprintf(
      'sh "./%s/procInput.sh" gallery',
      drupal_get_path('module', 'oerp_gallery')
    );

    exec($cmd);
    drupal_set_message('Photo(s) submitted.');
  }
}