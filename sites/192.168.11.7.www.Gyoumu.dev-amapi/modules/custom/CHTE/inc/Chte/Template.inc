<?php
namespace Chte;

class Template
{
  private $csvTemplate;
  private $csvSrc;
  private $xhtmlTheme;
  private $html_col;
  protected $nodeTheme;
  protected $nodeCsvTemplate;

  function __construct($theme_id, $tpl_id, $src_path)
  {
    $this->nodeTheme = node_load($theme_id);
    $this->xhtmlTheme = $this->nodeTheme->body;

    $this->nodeCsvTemplate = node_load($tpl_id);
    $this->csvTemplate = new \Gulei\Csv\Model(
      array(
           'source' => $this->nodeCsvTemplate->body,
      )
    );

    $this->html_col = $this->nodeCsvTemplate->field_chte_htmlmapping[0]['value'];
    $this->csvSrc = new \Gulei\Csv\Model($src_path);
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

  public function getPhptal()
  {
    $phptal = new \PHPTAL();
    $phptal->setSource($this->xhtmlTheme);
    return $phptal;
  }

  public function getHeaders()
  {
    return $this->csvTemplate->getHeaders();
  }

  public function getDefaults()
  {
    if (!$this->csvTemplate->getRow(0)) {
      return array_fill_keys(
        $this->csvTemplate->getHeaders(), null);
    }

    return array_combine(
      $this->csvTemplate->getHeaders(),
      $this->csvTemplate->getRow(0)
    );
  }

  public function getOutputTemplateRecs()
  {
    return $this->csvTemplate->connect();
  }

  public function getInputRecs()
  {
    return $this->csvSrc->connect();
  }

  public function getRecords()
  {
    preg_match('/([^-]*)-/', $this->nodeCsvTemplate->title, $m);

    $input = $this->getInputRecs();
    $defaults = $this->getDefaults();
    $headers = $this->csvSrc->getHeaders();

    switch ($m[1]) {
      case 'ebay':
        $proc = new \Chte\Processor\Ebay($input, $defaults, $headers);
        break;

      case 'yahoo':
        $proc = new \Chte\Processor\Yahoo($input, $defaults, $headers);
        break;

      case 'ylister':
        $proc = new \Chte\Processor\Ylister($input, $defaults, $headers);
        break;

      default:
        $proc = new \Chte\Processor\Record($input, $defaults, $headers);
    }

    return $proc->getRecords();
  }

  public function getAppliedRecs()
  {
    $recs = $this->getRecords();

    foreach ($recs as &$rec) {
      $html = $this->getHtmlByPhptal($rec);
      $html = $this->compressHtml($html);
      $rec[$this->html_col] = $html;
    }
    return $recs;
  }

  public function getAppliedCsv()
  {
    $appliedRecs = $this->getAppliedRecs();
    $hds = $this->getHeaders();
    $mask = array_fill_keys($hds, null);

    $recs = array();
    $recs[] = $hds;

    foreach ($appliedRecs as $appliedRec) {
      $recs[] = array_intersect_key($appliedRec, $mask);
    }

    $a = new \Gulei\_Array\Basic($recs);

    preg_match('/\(charset=([^\)]*)\)/', $this->nodeCsvTemplate->title, $m);
    return $a->dumpCsv($m[1]);
  }

  private function getHtmlByPhptal($input)
  {
    $tpl = $this->getPhptal();
    $tpl->title = $input['Title'];

    foreach ($input as $key => $val) {
      $var_name = str_replace(' ', '_', $key);

      if(strpos($var_name, '_') !== 0){
        $tpl->$var_name = $val;
      }
    }
    return $tpl->execute();
  }

  protected function compressHtml($src)
  {
    $src = str_replace(array("\r\n", "\r"), "\n", $src);
    $lines = explode("\n", $src);
    $new_lines = array();

    foreach ($lines as $i => $line) {
      if (!empty($line))
        $new_lines[] = trim($line);
    }
    return implode('', $new_lines);
  }

  static public function getTypeName($type)
  {
    $map = array(
      'chte_csv' => 'CHTE CSV',
      'chte_theme' => 'CHTE Theme',
    );
    return $map[$type];
  }
}