<?php
namespace Oerp\View\Prompt\Callee;

class getCached extends _Abstract
{
  public function isApplicable()
  {
    return !!($this->post['cache_id']);
  }

  public function execute()
  {
    $post = $this->post;
    $cache = cache_get($post['cache_id'], 'cache_oerpport')->data;
    $vname = preg_replace('/^formview:(.*)/', 'cache:${1}', $post['cache_id']);

    $overrides = \Oerp\JS\Helper::flatJsSend(json_decode($post['overrides']));
    $param = array(
      'fields' => $cache['fields'],
      'arch' => $cache['arch'],
      'prompt' => true,
      'new' => !$post['rid'],
      'hide' => array('toolbar-bottom'),
      'name' => $vname,
      'overrides' => $overrides,
    );

    $src = drupal_get_form(
      'oerp_ViewForm_form', $post['model'], 0, $post['rid'], $param);

    return drupal_json(rawurlencode($src));
  }
}