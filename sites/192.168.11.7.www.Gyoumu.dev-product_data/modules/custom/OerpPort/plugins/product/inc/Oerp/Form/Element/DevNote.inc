<?php
namespace Oerp\Form\Element;

class DevNote extends Ckeditor
{
  const BASEDIR_SUB = '_%BASE%_';

  private function saveExternalImg($src)
  {
    global $base_url;
    $src = str_replace($base_url . '/', self::BASEDIR_SUB, $src);

    if(strpos($src, self::BASEDIR_SUB) === 0){
      return $src;
    }

    $fp = urlencode($src);
    $path = sprintf('%s/external_img/%s', file_directory_path(), $fp);

    $saved = file_save_data(file_get_contents($src), $path, FILE_EXISTS_REPLACE);

    $toks = explode('/', $saved);
    $fname = urlencode(array_pop($toks));
    $toks[] = $fname;
    return self::BASEDIR_SUB . implode('/', $toks);
  }

  public function prepareSend()
  {
    $val = $this->args['value'];
    $pat = "/\< *[img][^\>]*[src] *= *[\"\']{0,1}([^\"\'\ >]*)/i";
    preg_match_all($pat, $val, $m);

    $links = array();
    foreach($m[1] as $src){
      $links[] = array(
        'src' => $src,
        'fp' => $this->saveExternalImg($src),
        );
    }

    foreach($links as $link){
      $val = str_replace($link['src'], $link['fp'], $val);
    }
    return $val;
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#value' : fld['value'],
  '#type' : 'DevNote'
};
JS;
  }

  protected function getEntryTemplate()
  {
    $ele = parent::getEntryTemplate();
    global $base_url;

    $ele['#default_value'] = str_replace(self::BASEDIR_SUB, $base_url . '/', $ele['#default_value']);
    return $ele;
  }
}