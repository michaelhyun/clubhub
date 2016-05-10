class AddCategoryToClub < ActiveRecord::Migration
  def change
    add_reference :clubs, :category, index: true
  end
end
