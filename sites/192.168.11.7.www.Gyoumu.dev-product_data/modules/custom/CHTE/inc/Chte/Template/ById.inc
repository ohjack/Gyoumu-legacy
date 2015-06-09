<?php
namespace Chte\Template;

class ById extends _Abstract
{
  protected $nodeTheme;
  protected $nodeCsvTemplate;

  function __construct($theme_id, $tpl_id, $src_path)
  {
    $this->nodeTheme = node_load($theme_id);
    $this->nodeCsvTemplate = node_load($tpl_id);

    parent::__construct(
      $this->nodeTheme->body,
      new \Gulei\Csv\Model(array('source' => $this->nodeCsvTemplate->body)),
      new \Gulei\Csv\Model($src_path),
      $this->nodeCsvTemplate->field_chte_htmlmapping[0]['value']
    );
  }

  protected function createProcessor($ns = null)
  {
    preg_match('/([^-]*)-/', $this->nodeCsvTemplate->title, $m);

    if($m[1]){
      return parent::createProcessor($m[1], $ns);
    }
    return parent::createProcessor();
  }

  static public function getThemeOpts()
  {
    $result = db_query("SELECT nid, title FROM {node} WHERE type='%s'", 'chte_theme');
    $themes = array();
    while ($theme = db_fetch_object($result)) {
      $themes[$theme->nid] = $theme->title;
    }
    asort($themes);
    return $themes;
  }

  static public function getTemplateOpts()
  {
    $result = db_query("SELECT nid, title FROM {node} WHERE type='%s'", 'chte_csv');
    $csvs = array();
    while ($csv = db_fetch_object($result)) {
      $csvs[$csv->nid] = $csv->title;
    }
    asort($csvs);
    return $csvs;
  }
}