<?php
namespace Chte\Importer;

class CsvInput extends \Gulei\Importer\Arch\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  public function getArch() {
    return <<<XML
<tree string="Get Rakuten CSV">
  <param name="no_arch">1</param>
</tree>
XML;
  }

  public function submit($form, &$form_state) {
    $values = $form_state['values'];
    $BufParam = $this->getBufferObj($form)->getParam();

    $m = new \Chte\Template\ById(
      FALSE,
      $values['csv_id'],
      $BufParam['filepath']
    );

    $download = file_save_data($m->getAppliedCsv(), '/tmp/output-' . date('Y-m-d') . '.csv');
    $form_state['storage']['redirect'] = '.' . $download;
  }

  public function getEles($form_state) {
    $StateCtrl = new \Gulei\Form\State($form_state);

    $eles = array();
    $eles['csv_id'] = array(
      '#type' => 'select',
      '#title' => 'CSV Template',
      '#options' => \Chte\Template\ById::getTemplateOpts(),
      '#default_value' => $StateCtrl->getVal('csv_id'),
    );
    return $eles;
  }

  public function theme($form) {
    $buf = '';
    $buf .= drupal_render($form['header']);
    $buf .= drupal_render($form['csv_file']);
    $buf .= drupal_render($form);
    return $buf;
  }
}