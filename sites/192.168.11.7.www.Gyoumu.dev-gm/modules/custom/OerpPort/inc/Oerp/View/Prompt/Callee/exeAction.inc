<?php
namespace Oerp\View\Prompt\Callee;

class exeAction extends _Abstract
{
  public function isApplicable()
  {
    return !!($this->post['act_id']);
  }

  public function execute()
  {
    $post = $this->post;
    $post['act_id'] = (int)$post['act_id'];
    $post['rid'] = (int)$post['rid'];

    $def = \Oerp\Query\Basic::_execute(
      array(
        'ir.actions.actions',
        'read',
        $post['act_id'],
        array('type')
      )
    );

    $act = \Oerp\Query\Basic::_execute(
      array(
        $def['type'],
        'read',
        $post['act_id'],
        false
      )
    );

    switch ($def['type']) {
      case 'ir.actions.wizard':
        $wiz_id = (int)\Oerp\Query\Wizard::_execute(
          array(
            $act['wiz_name'],
          ),
          'create'
        );

        $param = \Oerp\Query\Wizard::_execute(
          array(
            $wiz_id,
            array(
              'model' => $post['model'],
              'form' => array(),
              'id' => $post['rid'],
            ),
            'init'
          ),
          'execute'
        );

        $param['wid'] = $wiz_id;
        break;
    }

    $param['js'] = true;
    $param['hide'] = array('toolbar-bottom');

    $src = drupal_get_form(
      'oerp_ViewForm_form', $post['model'], 0, $post['rid'], $param
    );
    return drupal_json(rawurlencode($src));
  }
}