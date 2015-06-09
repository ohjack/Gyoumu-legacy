<?php
namespace Oerp\View\Prompt\Callee;

class getFormView extends _Abstract
{
  public function isApplicable()
  {
    return $this->post['type'] == 'form';
  }

  public function execute()
  {
    $post = $this->post;
    $q = new \Oerp\Query\Basic($post['model']);

    if ($post['vid']) {
      $view = $q->fields_view_get($post['vid'], $post['type']);
    }
    else {
      $view = $q->fields_view_get(false, $post['type']);
    }

    $overrides = \Oerp\JS\Helper::flatJsSend(json_decode($post['overrides']));

    $param = array(
      'prompt' => true,
      'new' => !$post['rid'],
      'hide' => array('toolbar-bottom'),
      'overrides' => $overrides,
    );

    $src = drupal_get_form(
      'oerp_ViewForm_form', $post['model'], $view['view_id'], $post['rid'], $param);

    return drupal_json(rawurlencode($src));
  }
}