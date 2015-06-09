<?php
namespace Yauc\Lister\Provider;

abstract class Abstract_Themed extends _Abstract
{
  protected function applyTemplate($rec)
  {
    static $theme;

    $suffix = $rec['theme'] ? '_' . $rec['theme'] : '';

    if(!$theme){
      $theme = file_get_contents(
        drupal_get_path('module', 'yauc_lister') . '/tpl/theme_' . $rec['sellerId'] . $suffix . '.xhtml');
    }

    unset($rec['category']);

    $recs = array(
      array_keys($rec),
      $rec,
    );

    static $provTpl;

    if(!$provTpl){
      $provTpl = new \Gulei\Csv\Model(array(
        'filepath' => drupal_get_path('module', 'yauc_lister') . '/tpl/upload.csv'
      ));
    }

    $ctrlTpl = new \Chte\Template\Basic(
      $theme,
      $provTpl,
      new \Gulei\Csv\Model(array('array' => $recs))
    );

    $rs = $ctrlTpl->getAppliedRecs();
    return $rs[0];
  }

  protected function prepareRecord($rec)
  {
    $rec = parent::prepareRecord($rec);
    $rec = array_merge($this->applyTemplate($rec), $rec);
    return $rec;
  }
}