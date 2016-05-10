class RemovecategoryFromclubs < ActiveRecord::Migration
  def change
  	remove_column :clubs, :category

  end
end
