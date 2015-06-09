<?php

require_once 'phpunit_setup_simple.inc';

class OerpTreeViewTest extends PHPUnit_Framework_TestCase
{
  public function testPracticalTest()
  {
    $ele = array(
      '#type' => 'oerp_treeview',
      '#param' =>
      array(
        'oerp:op-type' => 'edit',
        'vid' => '363',
        'model' => 'product.product',
        'searchpanel' => true,
        'cri_url' => NULL,
        'pager' => true,
      ),
      '#post' =>
      array(
      ),
      '#programmed' => false,
      '#tree' => false,
      '#parents' =>
      array(
        0 => 'view',
      ),
      '#array_parents' =>
      array(
        0 => 'view',
      ),
      '#weight' => 0,
      '#processed' => false,
      '#description' => NULL,
      '#attributes' =>
      array(
      ),
      '#required' => false,
      '#input' => true,
      '#value' => NULL,
      '#process' =>
      array(
        0 => 'oerp_treeview_process',
      ),
      '#name' => 'view',
      '#id' => 'edit-view',
    );

    $stub = $this->getMockBuilder('OerpTreeView')
        ->setMethods(array('getRecords'))
        ->setConstructorArgs(array($ele))
        ->getMock();

    $expHds = array(
      '#op' =>
      array(
        'data' => '<div class="oerp-button oerp-btn-add ui-state-default ui-corner-all">
  <span class="oerp-icon ui-icon ui-icon-document"></span>
</div>',
        'class' => 'oerp-op',
      ),
      'default_code' =>
      array(
        'data' => '<span class="gu_value">Code</span>',
        'gu_name' => 'default_code',
        'rowspan' => 1,
      ),
      'name' =>
      array(
        'data' => '<span class="gu_value">Name</span>',
        'gu_name' => 'name',
        'rowspan' => 1,
      ),
      'uom_id' =>
      array(
        'data' => '<span class="gu_value">Default UoM</span>',
        'gu_name' => 'uom_id',
        'rowspan' => 1,
      ),
      'qty_available' =>
      array(
        'data' => '<span class="gu_value">Real Stock</span>',
        'gu_name' => 'qty_available',
        'rowspan' => 1,
      ),
      'virtual_available' =>
      array(
        'data' => '<span class="gu_value">Virtual Stock</span>',
        'gu_name' => 'virtual_available',
        'rowspan' => 1,
      ),
      'state' =>
      array(
        'data' => '<span class="gu_value">Status</span>',
        'gu_name' => 'state',
        'rowspan' => 1,
      ),
    );

    $this->assertEquals($expHds, $stub->getHeaders());

    $expHdsAlt = array(
      '#op' =>
      array(
        'data' => '<div class="oerp-button oerp-btn-add ui-state-default ui-corner-all">
  <span class="oerp-icon ui-icon ui-icon-document"></span>
</div>',
        'class' => 'oerp-op',
      ),
      'default_code' =>
      array(
        'data' => '<span class="gu_value">Code</span>',
        'gu_name' => 'default_code',
        'rowspan' => 1,
      ),
      'name' =>
      array(
        'data' => '<span class="gu_value">Name</span>',
        'gu_name' => 'name',
        'rowspan' => 1,
      ),
      'uom_id' =>
      array(
        'data' => '<span class="gu_value">Default UoM</span>',
        'gu_name' => 'uom_id',
        'rowspan' => 1,
      ),
      'qty_available' =>
      array(
        'data' => '<span class="gu_value">Real Stock</span>',
        'gu_name' => 'qty_available',
        'rowspan' => 1,
      ),
      'virtual_available' =>
      array(
        'data' => '<span class="gu_value">Virtual Stock</span>',
        'gu_name' => 'virtual_available',
        'rowspan' => 1,
      ),
      'state' =>
      array(
        'data' => '<span class="gu_value">Status</span>',
        'gu_name' => 'state',
        'rowspan' => 1,
      ),
    );

    $this->assertEquals($expHdsAlt, $stub->getHeadersAltered());

    $expRecs = array(
      0 =>
      array(
        'uom_id' => 'PCE',
        'state' => 'In Development',
        'virtual_available' => 26,
        'default_code' => 'GR-00001',
        'qty_available' => 16,
        'id' => 2,
        'name' => 'Matte Black Front Grille - BMW e90/ e91 pre-facelift',
      ),
      1 =>
      array(
        'uom_id' => 'PCE',
        'state' => 'In Production',
        'virtual_available' => 2,
        'default_code' => 'GR-00002',
        'qty_available' => 2,
        'id' => 3,
        'name' => 'MERCEDES SLK R171 CHROME GRILLE',
      ),
      2 =>
      array(
        'uom_id' => 'PCE',
        'state' => 'In Production',
        'virtual_available' => 25,
        'default_code' => 'GR-00003',
        'qty_available' => 10,
        'id' => 4,
        'name' => 'BMW E53 X5 Front Grille Chrome',
      ),
    );

    $stub->expects($this->any())
        ->method('getRecords')
        ->will($this->returnValue($expRecs));

    $this->assertEquals($expRecs, $stub->getRecords());

    $expRecsAlt = array(
      0 =>
      array(
        'uom_id' => 'PCE',
        'state' => 'In Development',
        'virtual_available' => 26,
        'default_code' => 'GR-00001',
        'qty_available' => 16,
        'id' => 2,
        'name' => 'Matte Black Front Grille - BMW e90/ e91 pre-facelift',
      ),
      1 =>
      array(
        'uom_id' => 'PCE',
        'state' => 'In Production',
        'virtual_available' => 2,
        'default_code' => 'GR-00002',
        'qty_available' => 2,
        'id' => 3,
        'name' => 'MERCEDES SLK R171 CHROME GRILLE',
      ),
      2 =>
      array(
        'uom_id' => 'PCE',
        'state' => 'In Production',
        'virtual_available' => 25,
        'default_code' => 'GR-00003',
        'qty_available' => 10,
        'id' => 4,
        'name' => 'BMW E53 X5 Front Grille Chrome',
      ),
    );

    $this->assertEquals($expRecsAlt, $stub->getRecordsAltered());

    $expRows = array(
      0 =>
      array(
        'data' =>
        array(
          '#op' =>
          array(
            'data' => '
          <div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-pencil"></span>
          </div>
          <div class="oerp-button oerp-btn-del ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-close"></span>
          </div>
        ',
            'gu_name' => '#op',
            'rowspan' => 1,
            'class' => 'oerp-op',
            'no-nl2br' => 1,
          ),
          'default_code' =>
          array(
            'data' => 'GR-00001',
            'gu_name' => 'default_code',
            'rowspan' => 1,
            'oerp_field' => 'default_code',
          ),
          'name' =>
          array(
            'data' => 'Matte Black Front Grille - BMW e90/ e91 pre-facelift',
            'gu_name' => 'name',
            'rowspan' => 1,
            'oerp_field' => 'name',
          ),
          'uom_id' =>
          array(
            'data' => 'PCE',
            'gu_name' => 'uom_id',
            'rowspan' => 1,
            'oerp_field' => 'uom_id',
            'oerp_relation' => 'product.uom',
          ),
          'qty_available' =>
          array(
            'data' => 16,
            'gu_name' => 'qty_available',
            'rowspan' => 1,
            'oerp_field' => 'qty_available',
          ),
          'virtual_available' =>
          array(
            'data' => 26,
            'gu_name' => 'virtual_available',
            'rowspan' => 1,
            'oerp_field' => 'virtual_available',
          ),
          'state' =>
          array(
            'data' => 'In Development',
            'gu_name' => 'state',
            'rowspan' => 1,
            'oerp_field' => 'state',
          ),
        ),
        'class' => 'merge-odd',
        'oerp_rid' => 2,
      ),
      1 =>
      array(
        'data' =>
        array(
          '#op' =>
          array(
            'data' => '
          <div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-pencil"></span>
          </div>
          <div class="oerp-button oerp-btn-del ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-close"></span>
          </div>
        ',
            'gu_name' => '#op',
            'rowspan' => 1,
            'class' => 'oerp-op',
            'no-nl2br' => 1,
          ),
          'default_code' =>
          array(
            'data' => 'GR-00002',
            'gu_name' => 'default_code',
            'rowspan' => 1,
            'oerp_field' => 'default_code',
          ),
          'name' =>
          array(
            'data' => 'MERCEDES SLK R171 CHROME GRILLE',
            'gu_name' => 'name',
            'rowspan' => 1,
            'oerp_field' => 'name',
          ),
          'uom_id' =>
          array(
            'data' => 'PCE',
            'gu_name' => 'uom_id',
            'rowspan' => 1,
            'oerp_field' => 'uom_id',
            'oerp_relation' => 'product.uom',
          ),
          'qty_available' =>
          array(
            'data' => 2,
            'gu_name' => 'qty_available',
            'rowspan' => 1,
            'oerp_field' => 'qty_available',
          ),
          'virtual_available' =>
          array(
            'data' => 2,
            'gu_name' => 'virtual_available',
            'rowspan' => 1,
            'oerp_field' => 'virtual_available',
          ),
          'state' =>
          array(
            'data' => 'In Production',
            'gu_name' => 'state',
            'rowspan' => 1,
            'oerp_field' => 'state',
          ),
        ),
        'class' => 'merge-even',
        'oerp_rid' => 3,
      ),
      2 =>
      array(
        'data' =>
        array(
          '#op' =>
          array(
            'data' => '
          <div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-pencil"></span>
          </div>
          <div class="oerp-button oerp-btn-del ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-close"></span>
          </div>
        ',
            'gu_name' => '#op',
            'rowspan' => 1,
            'class' => 'oerp-op',
            'no-nl2br' => 1,
          ),
          'default_code' =>
          array(
            'data' => 'GR-00003',
            'gu_name' => 'default_code',
            'rowspan' => 1,
            'oerp_field' => 'default_code',
          ),
          'name' =>
          array(
            'data' => 'BMW E53 X5 Front Grille Chrome',
            'gu_name' => 'name',
            'rowspan' => 1,
            'oerp_field' => 'name',
          ),
          'uom_id' =>
          array(
            'data' => 'PCE',
            'gu_name' => 'uom_id',
            'rowspan' => 1,
            'oerp_field' => 'uom_id',
            'oerp_relation' => 'product.uom',
          ),
          'qty_available' =>
          array(
            'data' => 10,
            'gu_name' => 'qty_available',
            'rowspan' => 1,
            'oerp_field' => 'qty_available',
          ),
          'virtual_available' =>
          array(
            'data' => 25,
            'gu_name' => 'virtual_available',
            'rowspan' => 1,
            'oerp_field' => 'virtual_available',
          ),
          'state' =>
          array(
            'data' => 'In Production',
            'gu_name' => 'state',
            'rowspan' => 1,
            'oerp_field' => 'state',
          ),
        ),
        'class' => 'merge-odd',
        'oerp_rid' => 4,
      ),
    );

    $this->assertEquals($expRows, $stub->getRows());

    $expRowsAlt = array(
      0 =>
      array(
        'data' =>
        array(
          '#op' =>
          array(
            'data' => '<span class="gu_value">
          <div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-pencil"></span>
          </div>
          <div class="oerp-button oerp-btn-del ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-close"></span>
          </div>
        </span>',
            'gu_name' => '#op',
            'rowspan' => 1,
            'class' => 'oerp-op',
            'no-nl2br' => 1,
          ),
          'default_code' =>
          array(
            'data' => '<span class="gu_value">GR-00001</span>',
            'gu_name' => 'default_code',
            'rowspan' => 1,
            'oerp_field' => 'default_code',
          ),
          'name' =>
          array(
            'data' => '<span class="gu_value">Matte Black Front Grille - BMW e90/ e91 pre-facelift</span>',
            'gu_name' => 'name',
            'rowspan' => 1,
            'oerp_field' => 'name',
          ),
          'uom_id' =>
          array(
            'data' => '<span class="gu_value">PCE</span>',
            'gu_name' => 'uom_id',
            'rowspan' => 1,
            'oerp_field' => 'uom_id',
            'oerp_relation' => 'product.uom',
          ),
          'qty_available' =>
          array(
            'data' => '<span class="gu_value">16</span>',
            'gu_name' => 'qty_available',
            'rowspan' => 1,
            'oerp_field' => 'qty_available',
          ),
          'virtual_available' =>
          array(
            'data' => '<span class="gu_value">26</span>',
            'gu_name' => 'virtual_available',
            'rowspan' => 1,
            'oerp_field' => 'virtual_available',
          ),
          'state' =>
          array(
            'data' => '<span class="gu_value">In Development</span>',
            'gu_name' => 'state',
            'rowspan' => 1,
            'oerp_field' => 'state',
          ),
        ),
        'class' => 'merge-odd',
        'oerp_rid' => 2,
      ),
      1 =>
      array(
        'data' =>
        array(
          '#op' =>
          array(
            'data' => '<span class="gu_value">
          <div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-pencil"></span>
          </div>
          <div class="oerp-button oerp-btn-del ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-close"></span>
          </div>
        </span>',
            'gu_name' => '#op',
            'rowspan' => 1,
            'class' => 'oerp-op',
            'no-nl2br' => 1,
          ),
          'default_code' =>
          array(
            'data' => '<span class="gu_value">GR-00002</span>',
            'gu_name' => 'default_code',
            'rowspan' => 1,
            'oerp_field' => 'default_code',
          ),
          'name' =>
          array(
            'data' => '<span class="gu_value">MERCEDES SLK R171 CHROME GRILLE</span>',
            'gu_name' => 'name',
            'rowspan' => 1,
            'oerp_field' => 'name',
          ),
          'uom_id' =>
          array(
            'data' => '<span class="gu_value">PCE</span>',
            'gu_name' => 'uom_id',
            'rowspan' => 1,
            'oerp_field' => 'uom_id',
            'oerp_relation' => 'product.uom',
          ),
          'qty_available' =>
          array(
            'data' => '<span class="gu_value">2</span>',
            'gu_name' => 'qty_available',
            'rowspan' => 1,
            'oerp_field' => 'qty_available',
          ),
          'virtual_available' =>
          array(
            'data' => '<span class="gu_value">2</span>',
            'gu_name' => 'virtual_available',
            'rowspan' => 1,
            'oerp_field' => 'virtual_available',
          ),
          'state' =>
          array(
            'data' => '<span class="gu_value">In Production</span>',
            'gu_name' => 'state',
            'rowspan' => 1,
            'oerp_field' => 'state',
          ),
        ),
        'class' => 'merge-even',
        'oerp_rid' => 3,
      ),
      2 =>
      array(
        'data' =>
        array(
          '#op' =>
          array(
            'data' => '<span class="gu_value">
          <div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-pencil"></span>
          </div>
          <div class="oerp-button oerp-btn-del ui-state-default ui-corner-all">
            <span class="oerp-icon ui-icon ui-icon-close"></span>
          </div>
        </span>',
            'gu_name' => '#op',
            'rowspan' => 1,
            'class' => 'oerp-op',
            'no-nl2br' => 1,
          ),
          'default_code' =>
          array(
            'data' => '<span class="gu_value">GR-00003</span>',
            'gu_name' => 'default_code',
            'rowspan' => 1,
            'oerp_field' => 'default_code',
          ),
          'name' =>
          array(
            'data' => '<span class="gu_value">BMW E53 X5 Front Grille Chrome</span>',
            'gu_name' => 'name',
            'rowspan' => 1,
            'oerp_field' => 'name',
          ),
          'uom_id' =>
          array(
            'data' => '<span class="gu_value">PCE</span>',
            'gu_name' => 'uom_id',
            'rowspan' => 1,
            'oerp_field' => 'uom_id',
            'oerp_relation' => 'product.uom',
          ),
          'qty_available' =>
          array(
            'data' => '<span class="gu_value">10</span>',
            'gu_name' => 'qty_available',
            'rowspan' => 1,
            'oerp_field' => 'qty_available',
          ),
          'virtual_available' =>
          array(
            'data' => '<span class="gu_value">25</span>',
            'gu_name' => 'virtual_available',
            'rowspan' => 1,
            'oerp_field' => 'virtual_available',
          ),
          'state' =>
          array(
            'data' => '<span class="gu_value">In Production</span>',
            'gu_name' => 'state',
            'rowspan' => 1,
            'oerp_field' => 'state',
          ),
        ),
        'class' => 'merge-odd',
        'oerp_rid' => 4,
      ),
    );

    $this->assertEquals($expRowsAlt, $stub->getRowsAltered());
  }
}