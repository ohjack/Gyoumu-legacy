<?php
namespace Oerp\View\Prompt\Callee;

class getParentView extends _Abstract
{
  public function isApplicable()
  {
    if(!$this->post['par_model'])
      return false;

    $post = $this->post;
    $q = new \Oerp\Query\Basic($post['par_model']);
    $resp = $q->fields_view_get($post['par_vid'], 'form');
    $field = $resp['fields'][$post['name']];

    if ($form = $field['views']['form']) {
      $this->post['field'] = $field;
      $this->post['form'] = $form;
      $this->post['views']['form'] = $form;
      return true;
    }
    return false;
  }

  public function execute()
  {
    $post = $this->post;

    $param = array(
      'fields' => $post['form']['fields'],
      'arch' => $post['form']['arch'],
      'prompt' => true,
      'new' => !$post['rid'],
      'hide' => array('toolbar-bottom'),
    );

    $src = drupal_get_form(
      'oerp_ViewForm_form', $post['field']['relation'], 0, $post['rid'], $param);

    return drupal_json(rawurlencode($src));
  }
}