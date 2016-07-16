class AddClubsToCategories < ActiveRecord::Migration
  def change
    add_column :categories, :clubs, :string
  end
end
