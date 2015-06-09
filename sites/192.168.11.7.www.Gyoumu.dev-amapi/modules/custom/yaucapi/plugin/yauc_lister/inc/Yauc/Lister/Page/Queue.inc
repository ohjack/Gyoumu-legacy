<?php
namespace Yauc\Lister\Page;

class Queue extends \Gulei\Page\Form\Basic
{
  private function isStageQueue($status)
  {
    return !in_array(\Yauc\Lister\Provider\_Abstract::Q_DONE, $status)
        && !in_array(\Yauc\Lister\Provider\_Abstract::Q_CANCEL, $status);
  }

  public function hook_form(&$form_state, $sellerId, $status)
  {
    $arch = \Gulei\File\Helper::getPathContent('module', 'yauc_lister', '/xml/viewQueue.xml');
    $prov = new \Yauc\Lister\Provider\Queue($sellerId, $status);

    $form = array();
    $form['sel_id'] = array(
      '#type' => 'value',
      '#value' => $sellerId,
    );

    $form['title'] = array(
      '#type' => 'markup',
      '#value' => "<h1><em>$sellerId</em></h1>",
    );

    if($this->isStageQueue($status)){
      $t = new \Yauc\Lister\Table\QueueSelect($arch, $prov->getData());
      $form['view'] = $t->getFormEles();
      $form['sub:cancel'] = array(
        '#type' => 'submit',
        '#value' => 'Mark as CANCEL',
      );

      $form['sub:trial'] = array(
        '#type' => 'submit',
        '#value' => 'Trial Upload'
      );

      $form['sub:upload'] = array(
        '#type' => 'submit',
        '#value' => 'Upload',
      );
    }
    else{
      $t = new \GuArchTable($arch, $prov);
      $form['view'] = array(
        '#type' => 'markup',
        '#value' => $t->theme(),
      );
    }

    drupal_add_css(drupal_get_path('module', 'yauc_lister') . '/yauc_lister.css');
    return $form;
  }

  protected function proc_sub_cancel($form, &$form_state)
  {
    $sels = $this->getSelection();

    foreach($sels as $id){
      $c = new \Yauc\Lister\Query\Queue($id);
      $c->setCancel();
    }
    drupal_set_message('Canceled ' . count($sels) . ' queued record(s).');
  }

  protected function proc_sub_trial($form, &$form_state)
  {
    $uploader = new \Yauc\Lister\Listing\Helper($form['sel_id']['#value']);
    $uploader->upload(true);
  }

  protected function proc_sub_upload($form, &$form_state)
  {
    $uploader = new \Yauc\Lister\Listing\Helper($form['sel_id']['#value']);
    $uploader->upload(false);
  }
}