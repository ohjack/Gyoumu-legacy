Drupal.behaviors.yaucjpController = function(){
  $('a.sug_addr').click(function(e){
    e.preventDefault();
    $this = $(this);
    
    $id_prefix = '#edit-contact-form-addr-';
    
    $($id_prefix + 'todoufuken').val($this.find('span:eq(0)').text());
    $($id_prefix + 'sichouson').val($this.find('span:eq(1)').text());
    $($id_prefix + 'chouiki').val($this.find('span:eq(2)').text());    
  });
}