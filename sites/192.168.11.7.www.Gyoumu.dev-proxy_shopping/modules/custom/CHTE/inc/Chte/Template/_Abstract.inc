<?php
namespace Chte\Template;

abstract class _Abstract
{
  const DEFAULT_NAMESPACE = "Chte\\Processor\\";
  const DEFAULT_PROC_NAME = 'Record';

  protected $csvTemplate;
  protected $csvSrc;
  protected $xhtmlTheme;
  protected $html_col;

  public function __construct($xhtml, \Gulei\Csv\Model $csv_tpl, \Gulei\Csv\Model $csv_input, $html_col = 'HTML')
  {
    $this->xhtmlTheme = $xhtml;
    $this->csvTemplate = $csv_tpl;
    $this->csvSrc = $csv_input;
    $this->html_col = $html_col;
  }

  protected function createProcessor($name = null, $ns = null)
  {
    if (is_null($name)) {
      $name = self::DEFAULT_PROC_NAME;
    }

    if (is_null($ns)) {
      $ns = self::DEFAULT_NAMESPACE;
    }

    $input = $this->getInputRecs();
    $defaults = $this->getDefaults();
    $headers = $this->csvSrc->getHeaders();
    $classname = $ns . ucfirst($name);

    return new $classname($input, $defaults, $headers);
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
    $proc = $this->createProcessor();
    return $proc->getRecords();
  }

  public function getAppliedRecs()
  {
    $appliedRecs = $this->getRecords();

    if ($this->xhtmlTheme) {
      foreach ($appliedRecs as &$appliedRec) {
        $html = $this->getHtmlByPhptal($appliedRec);
        $html = $this->compressHtml($html);
        $appliedRec[$this->html_col] = $html;
      }
      unset($appliedRec);
    }

    $hds = $this->getHeaders();
    $mask = array_fill_keys($hds, null);
    $recs = array();

    foreach ($appliedRecs as $appliedRec) {
      $recs[] = array_intersect_key($appliedRec, $mask);
    }
    return $recs;
  }

  public function getAppliedCsv()
  {
    $appliedRecs = $this->getAppliedRecs();
    $hds = $this->getHeaders();

    $recs = array_merge(array($hds), $appliedRecs);
    $a = new \Gulei\_Array\Basic($recs);

    preg_match('/\(charset=([^\)]*)\)/', $this->nodeCsvTemplate->title, $m);
    return $a->dumpCsv($m[1]);
  }

  private function getHtmlByPhptal($input)
  {
    $tpl = $this->getPhptal();
    $tpl->title = $input['Title'];
    $vars = array();

    foreach ($input as $key => $val) {
      $var_name = str_replace(' ', '_', $key);

      if(strpos($var_name, '_') !== 0){
        $tpl->$var_name = $val;
        $vars[$var_name] = $val;
      }
    }
    $tpl->vars_ = $vars;
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