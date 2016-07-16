class RemoveCategoryFromClubs < ActiveRecord::Migration
  def change
    remove_column :clubs, :category, :string
  end
end
